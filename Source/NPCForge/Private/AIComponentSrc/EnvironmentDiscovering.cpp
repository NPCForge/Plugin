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
	return FString::Printf(TEXT("\"phase\": \"%s\""), *GameMode->GetPhase());
}

FString UAIComponent::ScanForNearbyEntities(const float Radius, const FVector &ScanLocation) const
{
	TArray<AActor*> OverlappingActors;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

	TArray<AActor*> IgnoredActors;
	IgnoredActors.Add(GetOwner());

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
