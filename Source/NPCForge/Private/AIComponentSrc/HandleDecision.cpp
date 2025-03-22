#include "AIComponent.h"

AActor* UAIComponent::FindNPCByName(const FString& NpcName) const
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
		if (const UAIComponent* AIComp = Actor->FindComponentByClass<UAIComponent>();
			AIComp && AIComp->UniqueName == NpcName)
		{
			UE_LOG(LogTemp, Log, TEXT("[UAIComponent::FindNPCByName]: Successfully found %s"), *AIComp->UniqueName);
			return Actor;
		}
	}

	return nullptr;
}

bool UAIComponent::MoveToNPC(AActor* NPC) const
{
	const APawn* OwnerPawn = Cast<APawn>(GetOwner());
	
	if (!OwnerPawn)
	{
		UE_LOG(LogTemp, Error, TEXT("[UAIComponent::MoveToNPC]: %s"), TEXT("Unable to find owner"));
		return false;
	}
	
	if (AAIController* AIController = Cast<AAIController>(OwnerPawn->GetController()))
	{
		AIController->MoveToActor(NPC, 5.0f);
	} else
	{
		UE_LOG(LogTemp, Error, TEXT("[UAIComponent::MoveToNPC]: %s"), TEXT("Unable to find AIController"));
		return false;
	}
	return true;
}

void UAIComponent::TalkToNPC(const AActor* NPC, const FString &Message)
{
	if (const UAIComponent* AIComp = NPC->FindComponentByClass<UAIComponent>())
	{
		SendMessageToNPC(AIComp->EntityChecksum, Message);
	}
}

void UAIComponent::HandleDecision(const FString& Response)
{
	UE_LOG(LogTemp, Log, TEXT("[UAIComponent::HandleDecision]: %s"), *Response);
	
	FString TalkToLine;
	FString MessageLine;

	if (Response.Split(TEXT("TalkTo: "), nullptr, &TalkToLine))
	{
		UE_LOG(LogTemp, Log, TEXT("[UAIComponent::HandleDecision]: Handle TalkTo Logic"));

		if (TalkToLine.Split(TEXT("\nMessage: "), &TalkToLine, &MessageLine))
		{
			const FString EntityName = TalkToLine.TrimStartAndEnd();
			const FString Message = MessageLine.TrimStartAndEnd();
			
			if (AActor* TargetActor = FindNPCByName(EntityName))
			{
				if (MoveToNPC(TargetActor))
				{
					TalkToNPC(TargetActor, Message);
				}
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("[UAIComponent::HandleDecision]: %s%s"), TEXT("Unable to find entity with name = "), *EntityName);
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[UAIComponent::HandleDecision]: Could not find 'Message:' part"));
		}
	} else
	{
		UE_LOG(LogTemp, Error, TEXT("[UAIComponent::HandleDecision]: %s"), TEXT("Unable to find an entity name"));
	}
}