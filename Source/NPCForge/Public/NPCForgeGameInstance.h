// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "WebSocketHandler.h"
#include "NPCForgeGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class NPCFORGE_API UNPCForgeGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	UWebSocketHandler* GetWebSocketHandler();

	virtual void Shutdown() override;

private:
	UPROPERTY()
	UWebSocketHandler* WebSocketHandlerInstance = nullptr;
};
