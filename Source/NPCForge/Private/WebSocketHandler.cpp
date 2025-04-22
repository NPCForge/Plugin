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
		OnMessageReceived.Broadcast(Message);
	});
    
	Socket->OnRawMessage().AddLambda([](const void* Data, SIZE_T Size, SIZE_T BytesRemaining) -> void {
	});
    
	Socket->OnMessageSent().AddLambda([](const FString& MessageString) -> void {
	});

	Socket->Connect();
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
	JsonBody->SetStringField("API_KEY", "VDCAjPZ8jhDmXfsSufW2oZyU8SFZi48dRhA8zyKUjSRU3T1aBZ7E8FFIjdEM2X1d");
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
