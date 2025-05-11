#include "AIComponent.h"

void UAIComponent::ParseChecksums(const FString& InputString, TArray<FString>& OutChecksums)
{
	FString TrimmedString = InputString;
	TrimmedString.RemoveFromStart("[");
	TrimmedString.RemoveFromEnd("]");

	TrimmedString.ParseIntoArray(OutChecksums, TEXT(", "), true);
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
			const FString EntityChecksums = TalkToLine.TrimStartAndEnd();
			const FString Message = MessageLine.TrimStartAndEnd();

			TArray<FString> ChecksumsArray;
			ParseChecksums(EntityChecksums, ChecksumsArray);

			if (ChecksumsArray.Contains("Everyone"))
			{
				TriggerSendMessageEvent(Message);
				bIsBusy = false;
			} else
			{
				UE_LOG(LogTemp, Error, TEXT("[UAIComponent::HandleDecision]: Could not find entity receiver"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[UAIComponent::HandleDecision]: Could not find 'Message:' part"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[UAIComponent::HandleDecision]: Unable to find an entity name: %s"), *Response);
	}
}