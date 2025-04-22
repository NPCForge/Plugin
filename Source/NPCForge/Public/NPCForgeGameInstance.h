// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "WebSocketHandler.h"
#include "SaveEntityState.h"
#include "Kismet/GameplayStatics.h"
#include "NPCForgeGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class NPCFORGE_API UNPCForgeGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UWebSocketHandler* GetWebSocketHandler();

	virtual void Shutdown() override;

	void SaveInstanceState() const;
	void LoadInstanceState();

private:
	UPROPERTY()
	UWebSocketHandler* WebSocketHandlerInstance = nullptr;

	bool bIsRegistered = false;
};
