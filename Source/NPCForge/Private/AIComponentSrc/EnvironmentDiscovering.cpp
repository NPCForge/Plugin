#include "AIComponent.h"

FString UAIComponent::ScanEnvironment()
{
	FString EnvironmentPrompt = "{";

	EnvironmentPrompt += GetPhase();
	// EnvironmentPrompt += ScanForNearbyEntities(50000, GetOwner()->GetActorLocation());

	// UE_LOG(LogTemp, Log, TEXT("[UAIComponent::ScanEnvironment]: %s"), *EnvironmentPrompt);
	return EnvironmentPrompt + "}";
}

FString UAIComponent::GetPhase()
{
	// Need to remove this, just using to manually changing phase
	
	if (CountDecisions == 0)
	{
		CurrentPhase = TEXT("Discussion");
	} else if (CountDecisions == 4)
	{
		CurrentPhase = TEXT("Voting");
	} else if (CountDecisions == 5)
	{
		CurrentPhase = TEXT("Night");
	} else if (CountDecisions == 6)
	{
		CurrentPhase = TEXT("None");
	}
	CountDecisions++;
	// return FString::Printf(TEXT("\"phase\": \"%s\""), *GameMode->GetPhase());
	return FString::Printf(TEXT("\"phase\": \"%s\""), *CurrentPhase);
}

FString UAIComponent::ScanForNearbyEntities(const float Radius, const FVector &ScanLocation) const
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

	FString PromptString = "Nearby Entities: {";

	// Get entities with UAIComponent
	for (const AActor* Actor : OverlappingActors)
	{
		if (const UAIComponent* AIComp = Actor->FindComponentByClass<UAIComponent>())
		{
			PromptString += "[Checksum = " + AIComp->EntityChecksum + "]";
		}
	}
	
	return PromptString + "}";
}
