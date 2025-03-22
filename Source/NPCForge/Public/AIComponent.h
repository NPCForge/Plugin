#pragma once

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
	void TriggerSendMessageEvent(FString Value);
	
	// Properties
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString PersonalityPrompt;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString UniqueName;

	FString EntityChecksum;
	
	// AI Message Handling
	UFUNCTION(BlueprintCallable)
	void SendMessageToNPC(const FString& ReceiverChecksum, const FString& Content);

	// UFUNCTION(BlueprintCallable)
	// TArray<FMessage> GetReceivedMessages() const;

	UFUNCTION()
	void HandleMessage(FMessage Message);

	UFUNCTION(BlueprintCallable, Category="AI")
	void DelayResponse(FMessage Message);


	// Environment Discovering
	FString ScanEnvironment();

	FString ScanForNearbyEntities(float Radius, const FVector &ScanLocation);


	// WebSocket Communication
	UFUNCTION()
	void HandleWebSocketMessage(const FString& Message);

	void TakeDecision(const FString& Prompt) const;
	void HandleDecision(const FString& Response);

	AActor* FindNPCByName(const FString& NpcName) const;

	bool MoveToNPC(AActor *NPC) const;
	void TalkToNPC(const AActor *NPC, const FString &Message);

	void AddAIController();

protected:
	// Base Class
	virtual void BeginPlay() override;

private:
	// Data Persistence
	void SaveEntityState() const;
	void LoadEntityState();

	FTimerHandle ResponseTimerHandle;
	
	// Attributes
	UPROPERTY()
	UWebSocketHandler* WebSocketHandler;
	bool bIsRegistered = false;
	bool bIsConnected = false;

	bool bIsBusy = false;
};
