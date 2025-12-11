#include "AIComponent.h"

UAIComponent::UAIComponent()
{
	GameMode = nullptr;
	WebSocketHandler = nullptr;
	PrimaryComponentTick.bCanEverTick = true;
}

void UAIComponent::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Log, TEXT("[UAIComponent::BeginPlay]: %s joined the game!"), *UniqueName);
	
	if (const UWorld* World = GetOwner()->GetWorld())
	{
		if (auto* MyGI = Cast<UNPCForgeGameInstance>(World->GetGameInstance()))
		{
			if (!WebSocketHandler)
			{
				WebSocketHandler = MyGI->GetWebSocketHandler();
				WebSocketHandler->OnMessageReceived.AddDynamic(this, &UAIComponent::HandleWebSocketMessage);
				WebSocketHandler->OnReady.AddDynamic(this, &UAIComponent::OnWebsocketReady);
			}
		} else
		{
			UE_LOG(LogTemp, Warning, TEXT("Not Found Game Instance"));
		}
	}

	GameMode = Cast<AMyGameMode>(GetWorld()->GetAuthGameMode());
}

void UAIComponent::CheckGameRole()
{
	RoleCheckElapsed += 0.2f;
	
	if (AActor* Owner = GetOwner();
		Owner && Owner->GetClass()->ImplementsInterface(UAIInterface::StaticClass()))
	{
		FString Role = IAIInterface::Execute_GetGameRole(Owner);

		UE_LOG(LogTemp, Warning, TEXT("[CheckGameRole] %s got role: %s"), *UniqueName, *Role);
		
		if (Role != "None")
		{
			CachedRole = Role;
			UE_LOG(LogTemp, Warning, TEXT("[CheckGameRole] %s - Final Role cached: %s"), *UniqueName, *Role);
			GetWorld()->GetTimerManager().ClearTimer(RoleCheckTimerHandle);

			const FString CombinedString = FString::Printf(TEXT("%s%s%d"), *UniqueName, *PersonalityPrompt, WebSocketHandler->ApiUserID);
			EntityChecksum = FMD5::HashAnsiString(*CombinedString);

			if (!WebSocketHandler->IsEntityRegistered(EntityChecksum)) {
				UE_LOG(LogTemp, Warning, TEXT("[CheckGameRole] %s - Registering on API with role: %s"), *UniqueName, *CachedRole);
				WebSocketHandler->RegisterEntityOnApi(UniqueName, PersonalityPrompt, EntityChecksum, CachedRole);
			} else {
				UE_LOG(LogTemp, Warning, TEXT("[CheckGameRole] %s - Already registered (checksum: %s)"), *UniqueName, *EntityChecksum);
			}
			bIsWebsocketConnected = true;
			
			return;
		}
	}

	if (RoleCheckElapsed >= 10.0f)
	{
		UE_LOG(LogTemp, Error, TEXT("Timeout waiting for GetGameRole() != None"));
		GetWorld()->GetTimerManager().ClearTimer(RoleCheckTimerHandle);
	}
}

void UAIComponent::OnWebsocketReady()
{
    RoleCheckElapsed = 0.0f;
    bIsWebsocketConnected = false;
    bIsBusy = false;

    GetWorld()->GetTimerManager().SetTimer(
            RoleCheckTimerHandle,
            this,
            &UAIComponent::CheckGameRole,
            0.2f,
            true
    );
}


void UAIComponent::TriggerSendMessageEvent(const FString Message, const FString Reasoning) const
{
	// UE_LOG(LogTemp, Error, TEXT("Broadcasting message: %s"), *Message);
	if (OnSendMessage.IsBound())
	{
		// UE_LOG(LogTemp, Error, TEXT("Sending: %s"), *Message);
		OnSendMessage.Broadcast(Message, Reasoning);
	}
}

void UAIComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void UAIComponent::TickComponent(
	const float DeltaTime, const ELevelTick TickType,
	FActorComponentTickFunction *ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!GetOwner())
		return;

	// UE_LOG(LogTemp, Warning, TEXT("[UAIComponent::TickComponent]: %s ticked"), *UniqueName);
	if (AActor* Owner = GetOwner();
		Owner && Owner->GetClass()->ImplementsInterface(UAIInterface::StaticClass()))
	{
		bool bIsCurrentlyAlive = IAIInterface::Execute_IsAlive(Owner);
		
		// UE_LOG(LogTemp, Warning, TEXT("[UAIComponent::TickComponent]: %s is alive: %d"), *UniqueName, bIsCurrentlyAlive);

		if (bWasAlive && !bIsCurrentlyAlive)
		{
			// L'IA vient de mourir
			UE_LOG(LogTemp, Warning, TEXT("[UAIComponent::TickComponent]: %s has died, notifying API and stopping communication"), *UniqueName);

			if (WebSocketHandler && !EntityChecksum.IsEmpty())
			{
				WebSocketHandler->NotifyEntityDeath(EntityChecksum);
			}

			bIsWebsocketConnected = false;
			bWasAlive = false;
			return;
		}

		// Si l'IA est morte, arrêter toute communication
		if (!bIsCurrentlyAlive)
		{
			return;
		}
	}
	
	// UE_LOG(LogTemp, Warning, TEXT("[UAIComponent::TickComponent]: %s is alive"), *UniqueName);

	FString CurrentPhase = GameMode ? GameMode->GetPhase() : FString();
	if (CurrentPhase != LastKnownPhase) {
		bHasVotedInCurrentPhase = false;
		bIsBusy = false;
		LastKnownPhase = CurrentPhase;
	}

	TimeSinceLastDecision += DeltaTime;

	if (bIsWebsocketConnected && !bIsBusy &&
		TimeSinceLastDecision >= DecisionInterval && CachedRole != "None") {
		bIsBusy = true;
		TimeSinceLastDecision = 0.0f;
		
		// UE_LOG(LogTemp, Warning, TEXT("[UAIComponent::TickComponent]: %s is making a decision"), *UniqueName);

		const FString EnvironmentPrompt = ScanEnvironment();

		if (CurrentPhase == TEXT("Voting") && bHasVotedInCurrentPhase) {
			return;
		}
		if (CurrentPhase == TEXT("Night") && CachedRole != "Werewolf" && bHasVotedInCurrentPhase)
		{
			return;
		}

		MakeDecision(EnvironmentPrompt);
	}
}
