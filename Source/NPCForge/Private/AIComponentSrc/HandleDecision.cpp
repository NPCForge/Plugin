#include "AIComponent.h"

AActor* UAIComponent::FindNPCByName(const FString& NpcName)
{
	TArray<AActor*> OverlappingActors;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

	TArray<AActor*> IgnoredActors;
	IgnoredActors.Add(GetOwner());

	// Do sphere overlap
	UKismetSystemLibrary::SphereOverlapActors(
		GetWorld(),
		GetOwner()->GetActorLocation(),
		50000,
		ObjectTypes,
		nullptr,
		IgnoredActors,
		OverlappingActors
	);
	
	// Get entities with UAIComponent
	for (AActor* Actor : OverlappingActors)
	{
		UAIComponent* AIComp = Actor->FindComponentByClass<UAIComponent>();
		if (AIComp && AIComp->UniqueName == NpcName)
		{
			UE_LOG(LogTemp, Log, TEXT("[NPCForge:FindNPCByName]: Successfully found %s"), *AIComp->UniqueName);
			return Actor;
		}
	}

	return nullptr;
}

void UAIComponent::HandleDecision(const FString& Response)
{
	UE_LOG(LogTemp, Log, TEXT("[NPCForge:HandleDecision]: %s"), *Response);


	FString TalkToLine;
	if (Response.Split(TEXT("\nTalkTo: "), nullptr, &TalkToLine))
	{
		const FString EntityName = TalkToLine.TrimStartAndEnd();
		AActor* TargetActor = FindNPCByName(EntityName);

		if (TargetActor)
		{
			APawn* OwnerPawn = Cast<APawn>(GetOwner());
			if (!OwnerPawn)
			{
				UE_LOG(LogTemp, Warning, TEXT("[NPCForge:HandleDecision]: %s"), TEXT("Unable to find owner"));
				return;
			}

			AAIController* AIController = Cast<AAIController>(OwnerPawn->GetController());
			if (AIController)
			{
				UE_LOG(LogTemp, Log, TEXT("[NPCForge:HandleDecision]: %s"), TEXT("MOVING"));
				AIController->MoveToActor(TargetActor, 5.0f);
			} else
			{
				UE_LOG(LogTemp, Warning, TEXT("[NPCForge:HandleDecision]: %s"), TEXT("Unable to find AIController"));
			}
			// implement talkto npc
		} else
		{
			UE_LOG(LogTemp, Warning, TEXT("[NPCForge:HandleDecision]: %s"), TEXT("Unable to find entity"));
		}
	} else
	{
		UE_LOG(LogTemp, Warning, TEXT("[NPCForge:HandleDecision]: %s"), TEXT("Unable to find an entity name"));
	}
}