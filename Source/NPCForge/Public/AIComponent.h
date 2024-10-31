#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AIComponent.generated.h"

UCLASS(ClassGroup=(AI), meta=(BlueprintSpawnableComponent, DisplayName="NPCForge", ToolTip="Mark as NPC"))
class NPCFORGE_API UAIComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UAIComponent();
	
	UPROPERTY(EditAnywhere, Category="Mouvement")
	float Amplitude;
	
	UPROPERTY(EditAnywhere, Category="Mouvement")
	float Speed;

	FVector InitialPosition;

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
