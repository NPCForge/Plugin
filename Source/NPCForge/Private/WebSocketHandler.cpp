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
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Message);

	UE_LOG(LogTemp, Log, TEXT("[UAIComponent::HandleWebSocketMessage]: Raw message = %s"), *Message);
	
	if (TSharedPtr<FJsonObject> JsonObject;
		FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
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
						ApiUserID = JsonObject->GetNumberField(TEXT("id"));
						bIsRegistered = true;
					}
					else if (Value == "Connect")
					{
						UE_LOG(LogTemp, Log, TEXT("[UAIComponent::HandleWebSocketMessage]: Handle Connection Logic"));
						SetToken(JsonObject->GetStringField(TEXT("token")));
						ApiUserID = JsonObject->GetNumberField(TEXT("id"));
					}
					else if (Value == "GetEntities")
					{
						UE_LOG(LogTemp, Log, TEXT("[UAIComponent::HandleWebSocketMessage]: Handle GetEntities Logic"));
						
						if (JsonObject->HasTypedField<EJson::Array>(TEXT("entities")))
						{
							TArray<TSharedPtr<FJsonValue>> EntitiesArray;
							EntitiesArray = JsonObject->GetArrayField(TEXT("entities"));

							for (const TSharedPtr<FJsonValue>& Val : EntitiesArray)
							{
								if (Val->Type == EJson::Object)
								{
									if (TSharedPtr<FJsonObject> EntityObject = Val->AsObject();
										EntityObject.IsValid())
									{
										FString Id = EntityObject->GetStringField(TEXT("id"));
										FString Checksum = EntityObject->GetStringField(TEXT("checksum"));

										UE_LOG(LogTemp, Log, TEXT("Entity ID: %s | Checksum: %s"), *Id, *Checksum);

										RegisterEntity(Checksum, Id);
									}
								}
							}
						}
						else
						{
							UE_LOG(LogTemp, Warning, TEXT("No 'entities' array found or not of array type."));
						}

						OnReady.Broadcast();
					}
					else if (Value == "CreateEntity")
					{
						UE_LOG(LogTemp, Log, TEXT("[UAIComponent::HandleWebSocketMessage]: Handle CreateEntity Logic"));
						RegisterEntity(JsonObject->GetStringField(TEXT("checksum")), JsonObject->GetStringField(TEXT("id")));
					}
					else
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

bool UWebSocketHandler::IsEntityRegistered(const FString& Checksum) const
{
	return RegisteredEntities.IsValid() && RegisteredEntities->Contains(Checksum);
}

void UWebSocketHandler::Close()
{
	DisconnectAPI();
	Socket->Close();
}

void UWebSocketHandler::SendMessage(const FString& Action, TSharedPtr<FJsonObject> JsonBody) const
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
	} else
	{
		UE_LOG(LogTemp, Error, TEXT("[UWebSocketHandler::SendMessage]: Failed to serialize JSON"));
	}
}

void UWebSocketHandler::SetToken(const FString& NewToken)
{
	UE_LOG(LogTemp, Log, TEXT("[UWebSocketHandler::SetToken]: Setting token: %s"), *NewToken);
	this->Token = NewToken;

	if (NewToken != "")
	{
		bIsConnected = true;
		SendMessage("GetEntities", nullptr);
	}
	else
		bIsConnected = false;
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

void UWebSocketHandler::RegisterEntity(const FString& Checksum, const FString& ID) const
{
	RegisteredEntities->Emplace(Checksum, ID);
	UE_LOG(LogTemp, Log, TEXT("[UAIComponent::HandleWebSocketMessage]: Registered entity %s on id : %s"), *Checksum, *ID);
}

void UWebSocketHandler::RegisterEntityOnApi(const FString &Name, const FString &Prompt, const FString &Checksum, const FString &Role) const
{
	const TSharedPtr<FJsonObject> JsonBody = MakeShareable(new FJsonObject());
	JsonBody->SetStringField("name", Name);
	JsonBody->SetStringField("prompt", Prompt);
	JsonBody->SetStringField("checksum", Checksum);
	JsonBody->SetStringField("role", Role);
	SendMessage("CreateEntity", JsonBody);
}


void UWebSocketHandler::RegisterAPI() const
{
	const TSharedPtr<FJsonObject> JsonBody = MakeShareable(new FJsonObject());
	JsonBody->SetStringField("API_KEY", "VDCAjPZ8jhDmXfsSufW2oZyU8SFZi48dRhA8zyKUjSRU3T1aBZ7E8FFIjdEM2X1d");
	JsonBody->SetStringField("identifier", "UserPlugin");
	JsonBody->SetStringField("password", "password");
	JsonBody->SetStringField("game_prompt", "You are playing a turn-based social deduction game: **Werewolf**. Goal: Identify the werewolves while protecting the innocent. Make decisions and statements based on that. At each turn, analyze the discussion and try to deduce who could be a werewolf. Do not forget this goal. Roleplay is secondary.");
	SendMessage("Register", JsonBody);
}

void UWebSocketHandler::ConnectAPI() const
{
	const TSharedPtr<FJsonObject> JsonBody = MakeShareable(new FJsonObject());
	JsonBody->SetStringField("identifier", "UserPlugin");
	JsonBody->SetStringField("password", "password");
	SendMessage("Connect", JsonBody);
}


void UWebSocketHandler::DisconnectAPI()
{
        TSharedPtr<FJsonObject> JsonBody = MakeShareable(new FJsonObject());
        SendMessage("Disconnect", JsonBody);
        SetToken("");
}

void UWebSocketHandler::Restart()
{
    UE_LOG(LogTemp, Log, TEXT("[UWebSocketHandler::Restart]: Restarting connection"));

	TSharedPtr<FJsonObject> JsonBody = MakeShareable(new FJsonObject());
	SendMessage("Restart", JsonBody);
}
