// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MyGameMode.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class NPCFORGE_API AMyGameMode : public AGameModeBase
{
	GENERATED_BODY()

	public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Game Mode")
	FString GetPhase() const;

	virtual FString GetPhase_Implementation() const;
};
