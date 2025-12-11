// Fill out your copyright notice in the Description page of Project Settings.


#include "NPCForgeGameInstance.h"

UWebSocketHandler* UNPCForgeGameInstance::GetWebSocketHandler()
{
	if (!WebSocketHandlerInstance)
	{
		WebSocketHandlerInstance = NewObject<UWebSocketHandler>(this);
		WebSocketHandlerInstance->AddToRoot();
		WebSocketHandlerInstance->Initialize();

		WebSocketHandlerInstance->RegisterAPI();
	}

	return WebSocketHandlerInstance;
}

void UNPCForgeGameInstance::ResetGameInstance()
{
	if (WebSocketHandlerInstance)
	{
		WebSocketHandlerInstance->Close();
		WebSocketHandlerInstance->RemoveFromRoot();
		WebSocketHandlerInstance = nullptr;
	}
}

void UNPCForgeGameInstance::Shutdown()
{
	Super::Shutdown();

	if (WebSocketHandlerInstance)
	{
		WebSocketHandlerInstance->Close();
		WebSocketHandlerInstance->RemoveFromRoot();
		WebSocketHandlerInstance = nullptr;
	}
}
