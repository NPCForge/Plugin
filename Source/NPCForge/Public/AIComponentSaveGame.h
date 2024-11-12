// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "AIComponentSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class NPCFORGE_API UAIComponentSaveGame : public USaveGame
{
	GENERATED_BODY()

	public:
		UPROPERTY(VisibleAnywhere, Category="SaveData")
		TMap<FString, FGuid> NPCUniqueIDs;
};
