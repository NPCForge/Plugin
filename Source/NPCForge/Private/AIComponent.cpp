#include "AIComponent.h"
#include "Kismet/GameplayStatics.h"
#include "HttpManager.h"

UAIComponent::UAIComponent()
{
	WebSocketHandler = NewObject<UWebSocketHandler>();

	if (WebSocketHandler)
	{
		WebSocketHandler->Initialize();
		WebSocketHandler->OnMessageReceived.AddDynamic(this, &UAIComponent::HandleWebSocketMessage);
	}
	
	PrimaryComponentTick.bCanEverTick = false;
}

void UAIComponent::BeginPlay()
{
	Super::BeginPlay();

	// Generate checksum from name + prompt
	FString CombinedString = FString::Printf(TEXT("%s%s"), *UniqueName, *PersonalityPrompt);
	UniqueID = FMD5::HashAnsiString(*CombinedString);
	
	if (UMessageManager* MessageManager = GetWorld()->GetSubsystem<UMessageManager>())
	{
		// Subscribe to message reception event
		MessageManager->NewMessageReceivedEvent.AddDynamic(this, &UAIComponent::HandleMessage);
	}
	
	// auto send message for debug purpose
	// SendMessageToNPC("5D00E2C44F143DF62A45699B8A116C34", "content");

	// Testing environment scanning
	// ScanEnvironment();
	
	
	WebSocketHandler->SendMessage("TakeDecision", "Hello from unreal engine!");
}

void UAIComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	WebSocketHandler->Close();
	
	Super::EndPlay(EndPlayReason);
}

void UAIComponent::SendMessageToNPC(const FString& ReceiverID, const FString& Content)
{
	if (UMessageManager* MessageManager = GetWorld()->GetSubsystem<UMessageManager>())
	{
		MessageManager->SendMessage(UniqueID, ReceiverID, Content);
	}
}

TArray<FMessage> UAIComponent::GetReceivedMessages() const
{
	if (UMessageManager* MessageManager = GetWorld()->GetSubsystem<UMessageManager>())
	{
		return MessageManager->GetMessagesForNPC(UniqueID);
	}
	return TArray<FMessage>();
}

void UAIComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Placeholder for functionality that runs each tick.
	if (GetOwner())
	{
		// Future functionality for NPCs could go here.
	}
}

void UAIComponent::ScanEnvironment()
{
	// scan for nearby entities with a 1000 radius (average), and actual owner position
	ScanForNearbyEntities(1000, GetOwner()->GetActorLocation());
}

void UAIComponent::ScanForNearbyEntities(float Radius, FVector ScanLocation)
{
	TArray<AActor*> OverlappingActors;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes; // Object type to detect
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn)); // Should maybe change, lets see what object type an entity can be?

	TArray<AActor*> IgnoredActors; // Actors to ignore (optional)
	IgnoredActors.Add(GetOwner()); // Ignore actor with component

	// Do sphere overlap
	UKismetSystemLibrary::SphereOverlapActors(
		GetWorld(),
		ScanLocation,
		Radius,
		ObjectTypes,
		nullptr, // Filter by class (null for all)
		IgnoredActors,
		OverlappingActors
	);

	// Get entities with UAIComponent
	NearbyEntities.Empty();
	for (AActor* Actor : OverlappingActors)
	{
		if (Actor->FindComponentByClass<UAIComponent>())
		{
			NearbyEntities.Add(Actor);
		}
	}

	// Log (optional)
	UE_LOG(LogTemp, Log, TEXT("Found %d entities with UAIComponent"), NearbyEntities.Num());
}

void UAIComponent::HandleMessage(FMessage Message)
{
	UE_LOG(LogTemp, Display, TEXT("FROM AI: Message reçu de %s : %s"), *Message.SenderID, *Message.Content);
}

void UAIComponent::HandleWebSocketMessage(const FString& Message)
{
	UE_LOG(LogTemp, Display, TEXT("Handled message: %s"), *Message);
}
