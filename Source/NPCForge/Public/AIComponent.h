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
	// Constructor: Sets default values and enables ticking for the component.
	UAIComponent();

	// Called every frame, if ticking is enabled on this component.
	// @param DeltaTime - The time in seconds since the last frame.
	// @param TickType - The type of tick (e.g., game logic or physics).
	// @param ThisTickFunction - Information about this specific tick function.
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Store the personality prompt for the NPC
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString PersonalityPrompt;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString UniqueName;
	
	UFUNCTION(BlueprintCallable)
	void SendMessageToNPC(const FString& ReceiverID, const FString& Content);

	UFUNCTION(BlueprintCallable)
	TArray<FMessage> GetReceivedMessages() const;

	UFUNCTION()
	void HandleMessage(FMessage Message);

	void ScanEnvironment();

	void ScanForNearbyEntities(float Radius, const FVector &ScanLocation);

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
	void HandleWebSocketMessage(const FString& Message);

protected:
	// Called when the game starts or when the component is spawned.
	virtual void BeginPlay() override;

private:
	void SaveEntityState() const;
	void LoadEntityState();
	
	TArray<AActor*> NearbyEntities;
	bool bIsRegistered = false;
	FString UniqueID;
	UWebSocketHandler* WebSocketHandler;
};
