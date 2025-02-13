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

bool UAIComponent::MoveToNPC(AActor* NPC)
{
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("[NPCForge:HandleDecision]: %s"), TEXT("Unable to find owner"));
		return false;
	}

	AAIController* AIController = Cast<AAIController>(OwnerPawn->GetController());
	if (AIController)
	{
		AIController->MoveToActor(NPC, 5.0f);
	} else
	{
		UE_LOG(LogTemp, Warning, TEXT("[NPCForge:HandleDecision]: %s"), TEXT("Unable to find AIController"));
		return false;
	}
	return true;
}

void UAIComponent::TalkToNPC(AActor* NPC)
{
	UAIComponent* AIComp = NPC->FindComponentByClass<UAIComponent>();

	if (AIComp)
	{
		SendMessageToNPC(AIComp->EntityChecksum, "Hello");
	}
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
			if (MoveToNPC(TargetActor))
			{
				// Add something to wait until npc has arrived at destination
				TalkToNPC(TargetActor);
			}
		} else
		{
			UE_LOG(LogTemp, Warning, TEXT("[NPCForge:HandleDecision]: %s"), TEXT("Unable to find entity"));
		}
	} else
	{
		UE_LOG(LogTemp, Warning, TEXT("[NPCForge:HandleDecision]: %s"), TEXT("Unable to find an entity name"));
	}
}