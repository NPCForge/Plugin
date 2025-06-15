// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "AIInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UAIInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class NPCFORGE_API IAIInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "AI")
	FString GetGameRole();
};
