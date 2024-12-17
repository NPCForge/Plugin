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

void UAIComponent::HandleWebSocketMessage(const FString& JsonString)
{
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

	TSharedPtr<FJsonObject> JsonObject;
	if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
	{
		for (const auto& Pair : JsonObject->Values)
		{
			FString Key = Pair.Key;

			if (Pair.Value->Type == EJson::String)
			{
				FString Value = Pair.Value->AsString();
				UE_LOG(LogTemp, Log, TEXT("Key: %s, Value: %s"), *Key, *Value);
				
				if (Key == "route")
				{
					if (Value == "Register")
					{
						UE_LOG(LogTemp, Log, TEXT("Handle Register Logic"));
						WebSocketHandler->Token = JsonObject->GetStringField(TEXT("token"));
					}
					else if (Value == "TakeDecision")
					{
						UE_LOG(LogTemp, Log, TEXT("Handle TakeDecision Logic"));
					}
				}
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to parse JSON: %s"), *JsonString);
	}
}
