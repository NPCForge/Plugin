﻿#include "AIComponent.h"

FString UAIComponent::ScanEnvironment()
{
	FString EnvironmentPrompt = "";

	EnvironmentPrompt += ScanForNearbyEntities(50000, GetOwner()->GetActorLocation());

	UE_LOG(LogTemp, Display, TEXT("[NPCForge:ScanEnvironment]: %s"), *EnvironmentPrompt);
	return EnvironmentPrompt;
}

FString UAIComponent::ScanForNearbyEntities(float Radius, const FVector &ScanLocation)
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
	for (AActor* Actor : OverlappingActors)
	{
		UAIComponent* AIComp = Actor->FindComponentByClass<UAIComponent>();
		if (AIComp)
		{
			PromptString += "[Name = " + AIComp->UniqueName + "]";
		}
	}
	
	return PromptString + "}";
}
