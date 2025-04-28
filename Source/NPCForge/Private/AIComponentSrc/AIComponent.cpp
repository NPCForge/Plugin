#include "AIComponent.h"

UAIComponent::UAIComponent()
{
	WebSocketHandler = nullptr;
	PrimaryComponentTick.bCanEverTick = true;
}

void UAIComponent::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Log, TEXT("[UAIComponent::BeginPlay]: %s joined the game!"), *UniqueName);

	if (UniqueName == "Pascal")
	{
		bIsBusy = true;
	}
	
	UWorld* World = GetOwner()->GetWorld();
	if (World)
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
	
	if (UMessageManager* MessageManager = GetWorld()->GetSubsystem<UMessageManager>())
	{
		MessageManager->NewMessageReceivedEvent.AddDynamic(this, &UAIComponent::HandleMessage);
	}
}

void UAIComponent::OnWebsocketReady()
{
	const FString CombinedString = FString::Printf(TEXT("%s%s%d"), *UniqueName, *PersonalityPrompt, WebSocketHandler->ApiUserID);
	EntityChecksum = FMD5::HashAnsiString(*CombinedString);
	
	if (!WebSocketHandler->IsEntityRegistered(EntityChecksum)) {
		WebSocketHandler->RegisterEntityOnApi(UniqueName, PersonalityPrompt, EntityChecksum);
	}
	bIsWebsocketConnected = true;
}


void UAIComponent::TriggerSendMessageEvent(FString Message)
{
	if (OnSendMessage.IsBound())
	{
		OnSendMessage.Broadcast(Message);
	}
}

void UAIComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void UAIComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	// Placeholder for functionality that runs each tick.
	if (GetOwner())
	{
		if (bIsWebsocketConnected && !bIsBusy)
		{
			bIsBusy = true;
			
			const FString EnvironmentPrompt = ScanEnvironment();
			
			TakeDecision(EnvironmentPrompt);
		}
	}
}
