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


void UAIComponent::TriggerSendMessageEvent(const FString Message) const
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

void UAIComponent::TickComponent(const float DeltaTime, const ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (!GetOwner()) return;

	TimeSinceLastDecision += DeltaTime;
    
	if (bIsWebsocketConnected && !bIsBusy && TimeSinceLastDecision >= DecisionInterval)
	{
		bIsBusy = true;
		TimeSinceLastDecision = 0.0f;

		const FString EnvironmentPrompt = ScanEnvironment();
		MakeDecision(EnvironmentPrompt);
	}

}
