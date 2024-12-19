#include "AIComponent.h"

UAIComponent::UAIComponent()
{
	WebSocketHandler = nullptr;
	PrimaryComponentTick.bCanEverTick = true;
}

void UAIComponent::BeginPlay()
{
	Super::BeginPlay();

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
	
	// auto send message for debug purpose
	// SendMessageToNPC("5D00E2C44F143DF62A45699B8A116C34", "content");

	// Testing environment scanning
	// ScanEnvironment();

	// if (EntityChecksum == "7543525dcb1f85031029a54e10cab089")
	// 	WebSocketHandler->SendMessage("TakeDecision", "Hello from unreal engine!");

	if (!bIsRegistered) {
		WebSocketHandler->RegisterAPI(EntityChecksum, UniqueName, PersonalityPrompt);
	} else {
		WebSocketHandler->ConnectAPI(EntityChecksum);
	}
}

void UAIComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	SaveEntityState();
	WebSocketHandler->Close();
	
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
			TakeDecision();
			bTmpDidTakeDecision = true;
		}
		// Future functionality for NPCs could go here.
	}
}
