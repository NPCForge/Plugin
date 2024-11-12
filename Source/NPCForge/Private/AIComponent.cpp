#include "AIComponent.h"
#include "AIComponentSaveGame.h"
#include "Kismet/GameplayStatics.h"

UAIComponent::UAIComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UAIComponent::BeginPlay()
{
	Super::BeginPlay();

	// Attempt to load a previously saved Unique ID.
	// If loading fails (i.e., the ID doesn't exist), generate a new Unique ID and save it.
	if (!LoadUniqueID())
	{
		UniqueID = FGuid::NewGuid();
		SaveUniqueID();
	}

	if (UMessageManager* MessageManager = GetWorld()->GetSubsystem<UMessageManager>())
	{
		// Abonnement à l'événement de réception de message
		MessageManager->NewMessageReceivedEvent.AddDynamic(this, &UAIComponent::HandleMessage);
	}

	UE_LOG(LogTemp, Display, TEXT("UniqueID: %s"), *UniqueID.ToString());

	// auto send message for debug purpose
	SendMessageToNPC("5D00E2C44F143DF62A45699B8A116C34", "content");
}

bool UAIComponent::LoadUniqueID()
{
	// Attempt to load the game instance from a specific save slot.
	if (UAIComponentSaveGame* LoadGameInstance = Cast<UAIComponentSaveGame>(UGameplayStatics::LoadGameFromSlot(TEXT("NPCSaveSlot"), 0)))
	{
		// Retrieve the name of the NPC (owner of this component).
		FString NPCName = GetOwner()->GetName();

		// Check if the Unique ID for this NPC name exists in the saved data.
		if (LoadGameInstance->NPCUniqueIDs.Contains(NPCName))
		{
			// Load the Unique ID associated with this NPC name.
			UniqueID = LoadGameInstance->NPCUniqueIDs[NPCName];
			return true;
		}
	}
	// Return false if loading failed or no Unique ID was found.
	return false;
}

void UAIComponent::SaveUniqueID() const
{
	// Attempt to load the existing save game instance or create a new one if it doesn't exist.
	UAIComponentSaveGame* SaveGameInstance = Cast<UAIComponentSaveGame>(UGameplayStatics::LoadGameFromSlot(TEXT("NPCSaveSlot"), 0));
	if (!SaveGameInstance)
	{
		// Create a new save game instance if none was loaded.
		SaveGameInstance = Cast<UAIComponentSaveGame>(UGameplayStatics::CreateSaveGameObject(UAIComponentSaveGame::StaticClass()));
	}

	// Retrieve the name of the NPC (owner of this component).
	const FString NPCName = GetOwner()->GetName();

	// Add or update the Unique ID for this NPC name in the save data.
	SaveGameInstance->NPCUniqueIDs.Add(NPCName, UniqueID);

	// Save the updated data back to the same save slot.
	UGameplayStatics::SaveGameToSlot(SaveGameInstance, TEXT("NPCSaveSlot"), 0);
}

void UAIComponent::SendMessageToNPC(const FString& ReceiverID, const FString& Content)
{
	if (UMessageManager* MessageManager = GetWorld()->GetSubsystem<UMessageManager>())
	{
		MessageManager->SendMessage(UniqueID.ToString(), ReceiverID, Content);
	}
}

TArray<FMessage> UAIComponent::GetReceivedMessages() const
{
	if (UMessageManager* MessageManager = GetWorld()->GetSubsystem<UMessageManager>())
	{
		return MessageManager->GetMessagesForNPC(UniqueID.ToString());
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

void UAIComponent::HandleMessage(FMessage Message)
{
	UE_LOG(LogTemp, Display, TEXT("FROM AI: Message reçu de %s : %s"), *Message.SenderID, *Message.Content);
}
