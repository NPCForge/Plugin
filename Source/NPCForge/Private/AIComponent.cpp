#include "AIComponent.h"

UAIComponent::UAIComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	
	Amplitude = 50.0f;
	Speed = 2.0f;
}

void UAIComponent::BeginPlay()
{
	Super::BeginPlay();
	
	InitialPosition = GetOwner()->GetActorLocation();
}

void UAIComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (GetOwner())
	{
		const float OffsetZ = Amplitude * FMath::Sin(Speed * GetWorld()->GetTimeSeconds());

		FVector NewPosition = InitialPosition;
		NewPosition.Z += OffsetZ;

		GetOwner()->SetActorLocation(NewPosition);
	}
}
