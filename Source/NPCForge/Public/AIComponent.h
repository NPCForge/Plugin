#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WebSocketHandler.h"
#include "SaveEntityState.h"
#include "Kismet/GameplayStatics.h"
#include "JsonObjectConverter.h"
#include "AIController.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "NPCForgeGameInstance.h"
#include "AIInterface.h"
#include "MyGameMode.h"

#include "AIComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSendMessage, FString, Message, FString, Reasoning);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnVote, FString, Voter, FString, VoteTarget);

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

	// Voting
	UPROPERTY(BlueprintAssignable, Category="Events")
	FOnVote OnVote;

	UFUNCTION(BlueprintCallable, Category="MyComponent")
	void TriggerSendMessageEvent(const FString Message, const FString Reasoning) const;
	
	// Properties
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString PersonalityPrompt;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString UniqueName;

	FString EntityChecksum;


	// Environment Discovering
	FString ScanEnvironment();

	FString GetPhase();

	FString ScanForNearbyEntities(const float Radius, const FVector &ScanLocation) const;


	// WebSocket Communication
	UFUNCTION()
	void HandleWebSocketMessage(const FString& JsonString);

	UFUNCTION()
	void OnWebsocketReady();

	void MakeDecision(const FString& Prompt) const;
	void HandleDecision(const TSharedPtr<FJsonObject> &JsonObject);
	
	static void ParseChecksums(const FString& InputString, TArray<FString>& OutChecksums);

	void CheckGameRole();
	
protected:
	// Base Class
	virtual void BeginPlay() override;

private:
	float TimeSinceLastDecision = 0.0f;
	float DecisionInterval = 5.0f;

	FTimerHandle RoleCheckTimerHandle;
	float RoleCheckElapsed = 0.0f;
	FString CachedRole = "None";

	AMyGameMode *GameMode;

	FTimerHandle ResponseTimerHandle;
	
	// Attributes
	UWebSocketHandler* WebSocketHandler;
	// bool bIsRegistered = false;
	bool bIsWebsocketConnected = false;

	bool bIsBusy = false;


	int CountDecisions = 0;
	FString CurrentPhase;
};
