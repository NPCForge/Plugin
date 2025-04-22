// Fill out your copyright notice in the Description page of Project Settings.


#include "NPCForgeGameInstance.h"

UWebSocketHandler* UNPCForgeGameInstance::GetWebSocketHandler()
{
	if (!WebSocketHandlerInstance)
	{
		WebSocketHandlerInstance = NewObject<UWebSocketHandler>(this);
		WebSocketHandlerInstance->AddToRoot();
		WebSocketHandlerInstance->Initialize();

		LoadInstanceState();

		if (!bIsRegistered)
		{
			WebSocketHandlerInstance->RegisterAPI();
		} else
		{
			WebSocketHandlerInstance->ConnectAPI();
			bIsRegistered = true;
		}
	}
	
	return WebSocketHandlerInstance;
}

void UNPCForgeGameInstance::Shutdown()
{
	if (WebSocketHandlerInstance)
	{
		WebSocketHandlerInstance->Close();
	}
	SaveInstanceState();
}

void UNPCForgeGameInstance::SaveInstanceState() const
{
	USaveEntityState* SaveGameInstance = Cast<USaveEntityState>(UGameplayStatics::CreateSaveGameObject(USaveEntityState::StaticClass()));

	SaveGameInstance->bIsRegistered = bIsRegistered;
	UE_LOG(LogTemp, Log, TEXT("[UAIComponent::SaveEntityState]: bIsRegistered value: %s"), bIsRegistered ? TEXT("true") : TEXT("false"));

	if (UGameplayStatics::SaveGameToSlot(SaveGameInstance, "GameInstance", 0))
	{
		UE_LOG(LogTemp, Log, TEXT("[UAIComponent::SaveEntityState]: Game saved successfully on slot: GameInstance"));
	}
}

void UNPCForgeGameInstance::LoadInstanceState()
{
	UE_LOG(LogTemp, Log, TEXT("[UAIComponent::LoadEntityState]: Try loading data for slot = GameInstance"));
	if (UGameplayStatics::DoesSaveGameExist("GameInstance", 0))
	{
		if (const USaveEntityState* LoadedGame = Cast<USaveEntityState>(UGameplayStatics::LoadGameFromSlot("GameInstance", 0)))
		{
			bIsRegistered = LoadedGame->bIsRegistered;
		}
	}
}
