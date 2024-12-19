#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MessageManager.h"
#include "WebSocketHandler.h"
#include "SaveEntityState.h"
#include "Kismet/GameplayStatics.h"
#include "JsonObjectConverter.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

#include "AIComponent.generated.h"

UCLASS(ClassGroup=(AI), meta=(BlueprintSpawnableComponent, DisplayName="NPCForge", ToolTip="Mark as NPC"))
class NPCFORGE_API UAIComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Base Class
	UAIComponent();
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	
	// Properties
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString PersonalityPrompt;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString UniqueName;

	
	// AI Message Handling
	UFUNCTION(BlueprintCallable)
	void SendMessageToNPC(const FString& ReceiverID, const FString& Content);

	UFUNCTION(BlueprintCallable)
	TArray<FMessage> GetReceivedMessages() const;

	UFUNCTION()
	void HandleMessage(FMessage Message);


	// Environment Discovering
	void ScanEnvironment();

	void ScanForNearbyEntities(float Radius, const FVector &ScanLocation);


	// WebSocket Communication
	UFUNCTION()
	void HandleWebSocketMessage(const FString& Message);

	void TakeDecision() const;

protected:
	// Base Class
	virtual void BeginPlay() override;

private:
	// Data Persistence
	void SaveEntityState() const;
	void LoadEntityState();

	// Attributes
	TArray<AActor*> NearbyEntities;
	UWebSocketHandler* WebSocketHandler;
	bool bIsRegistered = false;
	bool bIsConnected = false;
	FString EntityChecksum;


	bool bTmpDidTakeDecision = false;
};
