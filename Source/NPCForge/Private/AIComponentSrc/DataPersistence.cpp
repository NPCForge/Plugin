#include "AIComponent.h"

void UAIComponent::SaveEntityState() const
{
	USaveEntityState* SaveGameInstance = Cast<USaveEntityState>(UGameplayStatics::CreateSaveGameObject(USaveEntityState::StaticClass()));

	SaveGameInstance->bIsRegistered = bIsRegistered;

	if (UGameplayStatics::SaveGameToSlot(SaveGameInstance, *EntityChecksum, 0))
	{
		UE_LOG(LogTemp, Log, TEXT("[NPCForge:DataPersistence]: Game saved successfully on slot: %s"), *EntityChecksum);
	}
}

void UAIComponent::LoadEntityState()
{
	UE_LOG(LogTemp, Log, TEXT("[NPCForge:DataPersistence]: Try loading data for slot = %s"), *EntityChecksum);
	if (UGameplayStatics::DoesSaveGameExist(*EntityChecksum, 0))
	{
		if (const USaveEntityState* LoadedGame = Cast<USaveEntityState>(UGameplayStatics::LoadGameFromSlot(*EntityChecksum, 0)))
		{
			bIsRegistered = LoadedGame->bIsRegistered;
		}
	}
}
