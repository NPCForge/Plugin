// Fill out your copyright notice in the Description page of Project Settings.


#include "NPCForgeGameInstance.h"

UWebSocketHandler* UNPCForgeGameInstance::GetWebSocketHandler()
{
	if (!WebSocketHandlerInstance)
	{
		WebSocketHandlerInstance = NewObject<UWebSocketHandler>(this);
		WebSocketHandlerInstance->AddToRoot();
		WebSocketHandlerInstance->Initialize();
		
		WebSocketHandlerInstance->LoadInstanceState();

		if (!WebSocketHandlerInstance->bIsRegistered)
		{
			WebSocketHandlerInstance->RegisterAPI();
		} else
		{
			WebSocketHandlerInstance->ConnectAPI();
			WebSocketHandlerInstance->bIsRegistered = true;
		}
	}
	
	return WebSocketHandlerInstance;
}

void UNPCForgeGameInstance::Shutdown()
{
	Super::Shutdown();
	
	if (WebSocketHandlerInstance)
	{
		WebSocketHandlerInstance->SaveInstanceState();
		WebSocketHandlerInstance->Close();
		WebSocketHandlerInstance->RemoveFromRoot();
		WebSocketHandlerInstance = nullptr;
	}
}
