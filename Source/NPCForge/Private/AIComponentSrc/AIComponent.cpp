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

	if (!WebSocketHandler)
	{
		WebSocketHandler = NewObject<UWebSocketHandler>(this);
		if (WebSocketHandler)
		{
			WebSocketHandler->Initialize();
			WebSocketHandler->OnMessageReceived.AddDynamic(this, &UAIComponent::HandleWebSocketMessage);
		}
	}

	// Generate checksum from name + prompt
	const FString CombinedString = FString::Printf(TEXT("%s%s"), *UniqueName, *PersonalityPrompt);
	EntityChecksum = FMD5::HashAnsiString(*CombinedString);
	
	LoadEntityState();
	
	if (UMessageManager* MessageManager = GetWorld()->GetSubsystem<UMessageManager>())
	{
		// Subscribe to message reception event
		MessageManager->NewMessageReceivedEvent.AddDynamic(this, &UAIComponent::HandleMessage);
	}

	if (!bIsRegistered) {
		WebSocketHandler->RegisterAPI(EntityChecksum, UniqueName, PersonalityPrompt);
	} else {
		WebSocketHandler->ConnectAPI(EntityChecksum);
	}
}

void UAIComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	WebSocketHandler->Close();
	SaveEntityState();
	
	Super::EndPlay(EndPlayReason);
}

void UAIComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	// Placeholder for functionality that runs each tick.
	if (GetOwner())
	{
		if (bIsConnected && !bTmpDidTakeDecision)
		{
			const FString EnvironmentPrompt = ScanEnvironment();
			
			 TakeDecision(EnvironmentPrompt); 

			bTmpDidTakeDecision = true;
		}
	}
}
