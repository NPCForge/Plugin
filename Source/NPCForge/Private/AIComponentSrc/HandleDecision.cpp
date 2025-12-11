#include "AIComponent.h"

void UAIComponent::ParseChecksums(const FString &InputString,
                                  TArray<FString> &OutChecksums) {
  FString TrimmedString = InputString;
  TrimmedString.RemoveFromStart("[");
  TrimmedString.RemoveFromEnd("]");

  TrimmedString.ParseIntoArray(OutChecksums, TEXT(", "), true);
}

void UAIComponent::HandleDecision(const TSharedPtr<FJsonObject> &JsonObject) {
  const FString Action = JsonObject->GetStringField(TEXT("Action"));

  UE_LOG(LogTemp, Log, TEXT("[UAIComponent::HandleDecision]: Action = %s"),
         *Action);

  if (Action == TEXT("TalkTo")) {
    const FString Message = JsonObject->GetStringField(TEXT("Message"));
    const FString Reasoning = JsonObject->GetStringField(TEXT("Reasoning"));

    if (const FString Target = JsonObject->GetStringField(TEXT("TalkTo"));
        Target.Contains("Everyone")) {
      const float RandomDelay = FMath::RandRange(0.5f, 3.5f);

      GetWorld()->GetTimerManager().SetTimer(
        DelayedMessageTimerHandle,
        [this, Message, Reasoning]() {
          TriggerSendMessageEvent(Message, Reasoning);
          bIsBusy = false;
        },
        RandomDelay,
        false
      );
        } else {
          UE_LOG(
              LogTemp, Error,
              TEXT("[UAIComponent::HandleDecision]: Message is not for everyone"));
        }
  } else if (Action == TEXT("VoteFor")) {
    const FString Target = JsonObject->GetStringField(TEXT("VoteFor"));
    UE_LOG(LogTemp, Log, TEXT("%s vote for %s"), *UniqueName, *Target)
    OnVote.Broadcast(*UniqueName, *Target);
    bHasVotedInCurrentPhase = true;
    bIsBusy = false;
  } else {
    UE_LOG(LogTemp, Error, TEXT("Action not found"));
    bIsBusy = false;
  }
}