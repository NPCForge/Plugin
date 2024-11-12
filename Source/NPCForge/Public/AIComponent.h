#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MessageManager.h"
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

	// Loads the Unique ID of the NPC from a saved slot.
	// @return true if the Unique ID was successfully loaded, false otherwise.
	bool LoadUniqueID();

	// Saves the current Unique ID of the NPC to a save slot for persistence between sessions.
	void SaveUniqueID() const;

	// Store the personality prompt for the NPC
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString PersonalityPrompt;

	UFUNCTION(BlueprintCallable)
	void SendMessageToNPC(const FString& ReceiverID, const FString& Content);

	UFUNCTION(BlueprintCallable)
	TArray<FMessage> GetReceivedMessages() const;

	UFUNCTION()
	void HandleMessage(FMessage Message);

protected:
	// Called when the game starts or when the component is spawned.
	virtual void BeginPlay() override;

private:
	FGuid UniqueID;
};
