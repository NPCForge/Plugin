#include "AIComponent.h"

FString UAIComponent::GenerateEnvironmentPrompt()
{
	UE_LOG(LogTemp, Log, TEXT("[NPCForge:EnvironmentDiscovering]: Called"));
	
	for (AActor* Actor : NearbyEntities)
	{
		if (UAIComponent* AIComponent = Actor->FindComponentByClass<UAIComponent>())
		{
			FString EntityUniqueName = AIComponent->UniqueName;

			UE_LOG(LogTemp, Log, TEXT("[NPCForge:EnvironmentDiscovering]: Found entity with UniqueName: %s"), *EntityUniqueName);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[NPCForge:EnvironmentDiscovering]: Actor does not have a UAIComponent!"));
		}
	}
	return "";
}


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
		UAIComponent* AIComp = Actor->FindComponentByClass<UAIComponent>();
		if (AIComp)
		{
			UE_LOG(LogTemp, Log, TEXT("[NPCForge:EnvironmentDiscovering]: Actor %s has UAIComponent"), *Actor->GetName());
			NearbyEntities.Add(Actor);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[NPCForge:EnvironmentDiscovering]: Actor %s does not have UAIComponent"), *Actor->GetName());
		}
	}

	// Log (optional)
	UE_LOG(LogTemp, Log, TEXT("[NPCForge:EnvironmentDiscovering]: Found %d entities with UAIComponent"), NearbyEntities.Num());
}
