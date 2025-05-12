#include "AIComponent.h"

FString ExtractGptMessage(const FString& Message)
{
	const TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(Message);

	TSharedPtr<FJsonObject> JsonObject;

	if (FString GptResponse;
		FJsonSerializer::Deserialize(JsonReader, JsonObject) && JsonObject.IsValid())
	{
		if (FString OverallStatus;
			JsonObject->TryGetStringField(TEXT("status"), OverallStatus) && OverallStatus == TEXT("success"))
		{
			if (const TSharedPtr<FJsonObject> DataObject = JsonObject->GetObjectField(TEXT("data"));
				DataObject.IsValid())
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

void UAIComponent::MakeDecision(const FString& Prompt) const
{
	const TSharedPtr<FJsonObject> JsonBody = MakeShareable(new FJsonObject());
	JsonBody->SetStringField("message", Prompt);
	JsonBody->SetStringField("checksum", EntityChecksum);
	WebSocketHandler->SendMessage("MakeDecision", JsonBody);
}


void UAIComponent::HandleWebSocketMessage(const FString& JsonString)
{
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);
	
	if (TSharedPtr<FJsonObject> JsonObject;
		FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
	{
		if (JsonObject->GetStringField(TEXT("checksum")) != EntityChecksum)
		{
			return;
		}
		
		UE_LOG(LogTemp, Log, TEXT("[UAIComponent::HandleWebSocketMessage]: %s handling message for: %s"), *EntityChecksum, *JsonObject->GetStringField(TEXT("checksum")));

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
