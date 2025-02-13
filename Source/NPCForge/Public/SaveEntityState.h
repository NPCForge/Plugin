// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SaveEntityState.generated.h"

/**
 * 
 */
UCLASS()
class NPCFORGE_API USaveEntityState : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Category = "Entity Data")
	bool bIsRegistered;
};
