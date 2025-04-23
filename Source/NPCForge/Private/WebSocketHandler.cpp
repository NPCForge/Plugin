// Fill out your copyright notice in the Description page of Project Settings.


#include "WebSocketHandler.h"

void UWebSocketHandler::Initialize()
{
	Socket->OnConnected().AddLambda([]() -> void {
		UE_LOG(LogTemp, Log, TEXT("[UWebSocketHandler::Initialize]: Connected"));
	});
    
	Socket->OnConnectionError().AddLambda([](const FString & Error) -> void {
		UE_LOG(LogTemp, Log, TEXT("[UWebSocketHandler::Initialize]: Connection error: %s"), *Error);
	});
    
	Socket->OnClosed().AddLambda([](int32 StatusCode, const FString& Reason, bool bWasClean) -> void {
		UE_LOG(LogTemp, Log, TEXT("[UWebSocketHandler::Initialize]: Connection closed: %s"), *Reason);
	});
    
	Socket->OnMessage().AddLambda([this](const FString& Message) -> void {
		HandleReceivedMessage(Message);
	});
    
	Socket->OnRawMessage().AddLambda([](const void* Data, SIZE_T Size, SIZE_T BytesRemaining) -> void {
	});
    
	Socket->OnMessageSent().AddLambda([](const FString& MessageString) -> void {
	});

	Socket->Connect();
}

void UWebSocketHandler::HandleReceivedMessage(const FString &Message)
{
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Message);

	TSharedPtr<FJsonObject> JsonObject;
	if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
	{
		if (JsonObject->GetStringField(TEXT("status")) == TEXT("error"))
		{
			OnMessageReceived.Broadcast(Message);
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
					if (Value == "Register")
					{
						UE_LOG(LogTemp, Log, TEXT("[UAIComponent::HandleWebSocketMessage]: Handle Register Logic"));
						SetToken(JsonObject->GetStringField(TEXT("token")));
						bIsRegistered = true;
					}
					else if (Value == "Connect")
					{
						UE_LOG(LogTemp, Log, TEXT("[UAIComponent::HandleWebSocketMessage]: Handle Connection Logic"));
						SetToken(JsonObject->GetStringField(TEXT("token")));
					} else
					{
						OnMessageReceived.Broadcast(Message);
					}
				}
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[NPCForge:WebSocketCommunication]: Failed to parse JSON: %s"), *Message);
	}
}

void UWebSocketHandler::Close()
{
	// DisconnectAPI();
	Socket->Close();
}

void UWebSocketHandler::ResetGame()
{
	SendResetMessage();
}

void UWebSocketHandler::SendResetMessage()
{
	const TSharedPtr<FJsonObject> JsonBody = MakeShareable(new FJsonObject());
	JsonBody->SetStringField("action", "ResetGame");

	FString OutputString;

	if (const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
		FJsonSerializer::Serialize(JsonBody.ToSharedRef(), Writer)) {
		Socket->Send(OutputString);
		UE_LOG(LogTemp, Log, TEXT("[UWebSocketHandler::SendMessage]: JSON message sent: %s"), *OutputString);
	} else {
		UE_LOG(LogTemp, Error, TEXT("[UWebSocketHandler::SendMessage]: Failed to serialize JSON"));
	}
}


void UWebSocketHandler::SendMessage(const FString& Action, TSharedPtr<FJsonObject> JsonBody)
{
	if (!JsonBody.IsValid())
	{
		JsonBody = MakeShareable(new FJsonObject());
	}
	JsonBody->SetStringField("action", Action);
	JsonBody->SetStringField("token", Token);

	FString OutputString;
	if (const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
		FJsonSerializer::Serialize(JsonBody.ToSharedRef(), Writer)) {
		Socket->Send(OutputString);
		UE_LOG(LogTemp, Log, TEXT("[UWebSocketHandler::SendMessage]: JSON message sent: %s"), *OutputString);
	} else {
		UE_LOG(LogTemp, Error, TEXT("[UWebSocketHandler::SendMessage]: Failed to serialize JSON"));
	}
}

void UWebSocketHandler::SetToken(const FString& NewToken)
{
	UE_LOG(LogTemp, Log, TEXT("[UWebSocketHandler::SetToken]: Setting token: %s"), *NewToken);
	this->Token = NewToken;
}

void UWebSocketHandler::SaveInstanceState() const
{
	USaveEntityState* SaveGameInstance = Cast<USaveEntityState>(UGameplayStatics::CreateSaveGameObject(USaveEntityState::StaticClass()));

	SaveGameInstance->bIsRegistered = bIsRegistered;
	UE_LOG(LogTemp, Log, TEXT("[UAIComponent::SaveEntityState]: bIsRegistered value: %s"), bIsRegistered ? TEXT("true") : TEXT("false"));

	if (UGameplayStatics::SaveGameToSlot(SaveGameInstance, "GameInstance", 0))
	{
		UE_LOG(LogTemp, Log, TEXT("[UAIComponent::SaveEntityState]: Game saved successfully on slot: GameInstance"));
	}
}

void UWebSocketHandler::LoadInstanceState()
{
	UE_LOG(LogTemp, Log, TEXT("[UAIComponent::LoadEntityState]: Try loading data for slot = GameInstance"));
	if (UGameplayStatics::DoesSaveGameExist("GameInstance", 0))
	{
		if (const USaveEntityState* LoadedGame = Cast<USaveEntityState>(UGameplayStatics::LoadGameFromSlot("GameInstance", 0)))
		{
			bIsRegistered = LoadedGame->bIsRegistered;
		}
	}
}



// void UWebSocketHandler::ConnectAPI(const FString& Checksum)
// {
// 	TSharedPtr<FJsonObject> JsonBody = MakeShareable(new FJsonObject());
// 	JsonBody->SetStringField("checksum", Checksum);
// 	SendMessage("Connection", JsonBody);
// }

// void UWebSocketHandler::RegisterAPI(const FString& Checksum, const FString& Name, const FString& Prompt)
// {
// 	TSharedPtr<FJsonObject> JsonBody = MakeShareable(new FJsonObject());
// 	JsonBody->SetStringField("checksum", Checksum);
// 	JsonBody->SetStringField("API_KEY", "VDCAjPZ8jhDmXfsSufW2oZyU8SFZi48dRhA8zyKUjSRU3T1aBZ7E8FFIjdEM2X1d");
// 	JsonBody->SetStringField("name", Name);
// 	JsonBody->SetStringField("prompt", Prompt);
// 	SendMessage("Register", JsonBody);
// }

void UWebSocketHandler::RegisterAPI()
{
	const TSharedPtr<FJsonObject> JsonBody = MakeShareable(new FJsonObject());
	JsonBody->SetStringField("API_KEY", "VDCAjPZ8jhDmXfsSufW2oZyU8SFZi48dRhA8zyKUjSRU3T1aBZ7E8FFIjdEM2X1d");
	JsonBody->SetStringField("identifier", "Test");
	JsonBody->SetStringField("password", "passTest");
	SendMessage("Register", JsonBody);
}

void UWebSocketHandler::ConnectAPI()
{
	const TSharedPtr<FJsonObject> JsonBody = MakeShareable(new FJsonObject());
	JsonBody->SetStringField("identifier", "Test");
	JsonBody->SetStringField("password", "passTest");
	SendMessage("Connect", JsonBody);
}


// void UWebSocketHandler::DisconnectAPI()
// {
// 	TSharedPtr<FJsonObject> JsonBody = MakeShareable(new FJsonObject());
// 	SendMessage("Disconnect", JsonBody);
// 	SetToken("");
// }
