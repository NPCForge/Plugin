﻿#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MessageManager.h"
#include "WebSocketHandler.h"
#include "SaveEntityState.h"
#include "Kismet/GameplayStatics.h"
#include "JsonObjectConverter.h"
#include "AIController.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "NPCForgeGameInstance.h"

#include "AIComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSendMessage, FString, Message);

UCLASS(ClassGroup=(AI), meta=(BlueprintSpawnableComponent, DisplayName="NPCForge", ToolTip="Mark as NPC"))
class NPCFORGE_API UAIComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Base Class
	UAIComponent();
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// Display Chat
	UPROPERTY(BlueprintAssignable, Category="Events")
	FOnSendMessage OnSendMessage;

	UFUNCTION(BlueprintCallable, Category="MyComponent")
	void TriggerSendMessageEvent(const FString Message) const;
	
	// Properties
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString PersonalityPrompt;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString UniqueName;

	FString EntityChecksum;


	// Environment Discovering
	FString ScanEnvironment();

	FString ScanForNearbyEntities(const float Radius, const FVector &ScanLocation) const;


	// WebSocket Communication
	UFUNCTION()
	void HandleWebSocketMessage(const FString& JsonString);

	UFUNCTION()
	void OnWebsocketReady();

	void MakeDecision(const FString& Prompt) const;
	void HandleDecision(const FString& Response);
	
	static void ParseChecksums(const FString& InputString, TArray<FString>& OutChecksums);

protected:
	// Base Class
	virtual void BeginPlay() override;

private:
	float TimeSinceLastDecision = 0.0f;
	float DecisionInterval = 3.0f;

	FTimerHandle ResponseTimerHandle;
	
	// Attributes
	UWebSocketHandler* WebSocketHandler;
	// bool bIsRegistered = false;
	bool bIsWebsocketConnected = false;

	bool bIsBusy = false;
};
