#include "AIComponent.h"
#include "WorldPartition/ContentBundle/ContentBundleLog.h"

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
			UE_LOG(LogTemp, Log, TEXT("[UAIComponent::FindNPCByName]: Successfully found %s"), *AIComp->UniqueName);
			return Actor;
		}
	}

	return nullptr;
}

void UAIComponent::ParseNames(const FString& InputString, TArray<FString>& OutNames)
{
	FString TrimmedString = InputString;
	TrimmedString.RemoveFromStart("[");
	TrimmedString.RemoveFromEnd("]");

	TrimmedString.ParseIntoArray(OutNames, TEXT(", "), true);
}

void UAIComponent::TalkToNPC(AActor* NPC, FString Message,TArray<FString>& ReceiversNames)
{
	UAIComponent* AIComp = NPC->FindComponentByClass<UAIComponent>();

	if (AIComp)
	{
		SendMessageToNPC(AIComp->EntityChecksum, Message, ReceiversNames);
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
			FString EntityNames = TalkToLine.TrimStartAndEnd();
			FString Message = MessageLine.TrimStartAndEnd();

			TArray<FString> NamesArray;
			ParseNames(EntityNames, NamesArray);

			for (const FString& EntityName : NamesArray)
			{
				AActor* TargetActor = FindNPCByName(EntityName);
				
				if (TargetActor)
				{
					TalkToNPC(TargetActor, Message, NamesArray);
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("[UAIComponent::HandleDecision]: %s"), TEXT("Unable to find entity"));
				}
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