#include "AIComponent.h"

void UAIComponent::ScanEnvironment()
{
	// scan for nearby entities with a 1000 radius (average), and actual owner position
	ScanForNearbyEntities(1000, GetOwner()->GetActorLocation());
}

void UAIComponent::ScanForNearbyEntities(float Radius, const FVector &ScanLocation)
{
	TArray<AActor*> OverlappingActors;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes; // Object type to detect
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn)); // Should maybe change, lets see what object type an entity can be?

	TArray<AActor*> IgnoredActors; // Actors to ignore (optional)
	IgnoredActors.Add(GetOwner()); // Ignore actor with component

	// Do sphere overlap
	UKismetSystemLibrary::SphereOverlapActors(
		GetWorld(),
		ScanLocation,
		Radius,
		ObjectTypes,
		nullptr, // Filter by class (null for all)
		IgnoredActors,
		OverlappingActors
	);

	// Get entities with UAIComponent
	NearbyEntities.Empty();
	for (AActor* Actor : OverlappingActors)
	{
		if (Actor->FindComponentByClass<UAIComponent>())
		{
			NearbyEntities.Add(Actor);
		}
	}

	// Log (optional)
	UE_LOG(LogTemp, Log, TEXT("Found %d entities with UAIComponent"), NearbyEntities.Num());
}
