#include "AIComponent.h"

FString ExtractGptMessage(const FString& Message)
{
	FString GptResponse;

	TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(Message);

	TSharedPtr<FJsonObject> JsonObject;

	if (FJsonSerializer::Deserialize(JsonReader, JsonObject) && JsonObject.IsValid())
	{
		FString OverallStatus;
		if (JsonObject->TryGetStringField(TEXT("status"), OverallStatus) && OverallStatus == TEXT("success"))
		{
			TSharedPtr<FJsonObject> DataObject = JsonObject->GetObjectField(TEXT("data"));
			if (DataObject.IsValid())
			{
				if (DataObject->TryGetStringField(TEXT("message"), GptResponse))
				{
					return GptResponse;
				}
			}
		}
	}

	return FString();
}

void UAIComponent::TakeDecision(const FString& Prompt) const
{
	TSharedPtr<FJsonObject> JsonBody = MakeShareable(new FJsonObject());
	JsonBody->SetStringField("message", Prompt);
	JsonBody->SetStringField("checksum", EntityChecksum);
	WebSocketHandler->SendMessage("TakeDecision", JsonBody);
}


void UAIComponent::HandleWebSocketMessage(const FString& JsonString)
{
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

	TSharedPtr<FJsonObject> JsonObject;
	if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
	{
		if (JsonObject->GetStringField(TEXT("status")) == TEXT("error"))
		{
			UE_LOG(LogTemp, Error, TEXT("[UAIComponent::HandleWebSocketMessage]: Error received from API: %s"), *JsonObject->GetStringField(TEXT("message")));
			bIsBusy = false;
			return;
		}
		for (const auto& Pair : JsonObject->Values)
		{
			FString Key = Pair.Key;

			if (Pair.Value->Type == EJson::String)
			{
				FString Value = Pair.Value->AsString();
				
				if (Key == "route")
				{
					if (Value == "MakeDecision")
					{
						UE_LOG(LogTemp, Log, TEXT("[UAIComponent::HandleWebSocketMessage]: Handle MakeDecision Logic"));
						HandleDecision(*JsonObject->GetStringField(TEXT("message")));
					}
				}
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[NPCForge:WebSocketCommunication]: Failed to parse JSON: %s"), *JsonString);
	}
}
