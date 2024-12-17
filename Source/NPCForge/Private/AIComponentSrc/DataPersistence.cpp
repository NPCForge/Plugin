#include "AIComponent.h"

void UAIComponent::SaveEntityState() const
{
	USaveEntityState* SaveGameInstance = Cast<USaveEntityState>(UGameplayStatics::CreateSaveGameObject(USaveEntityState::StaticClass()));

	SaveGameInstance->bIsRegistered = bIsRegistered;

	if (UGameplayStatics::SaveGameToSlot(SaveGameInstance, TEXT("SaveSlot1"), 0))
	{
		UE_LOG(LogTemp, Log, TEXT("Game saved successfully!"));
	}
}

void UAIComponent::LoadEntityState()
{
	if (UGameplayStatics::DoesSaveGameExist(TEXT("SaveSlot1"), 0))
	{
		USaveEntityState* LoadedGame = Cast<USaveEntityState>(UGameplayStatics::LoadGameFromSlot(TEXT("SaveSlot1"), 0));

		if (LoadedGame)
		{
			// bIsRegistered = LoadedGame->bIsRegistered;
			bIsRegistered = false;
		}
	}
}
