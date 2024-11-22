// Fill out your copyright notice in the Description page of Project Settings.


#include "WebSocketHandler.h"

void UWebSocketHandler::Initialize()
{
	// We bind all available events
	Socket->OnConnected().AddLambda([]() -> void {
		UE_LOG(LogTemp, Display, TEXT("Connected"));
	});
    
	Socket->OnConnectionError().AddLambda([](const FString & Error) -> void {
		UE_LOG(LogTemp, Display, TEXT("Connection error: %s"), *Error);
	});
    
	Socket->OnClosed().AddLambda([](int32 StatusCode, const FString& Reason, bool bWasClean) -> void {
		// This code will run when the connection to the server has been terminated.
		// Because of an error or a call to Socket->Close().
			UE_LOG(LogTemp, Display, TEXT("Connection closed: %s"), *Reason);
	});
    
	Socket->OnMessage().AddLambda([this](const FString& Message) -> void {
		// Log le message reçu
		UE_LOG(LogTemp, Display, TEXT("%s"), *Message);

		OnMessageReceived.Broadcast(Message);
	});
    
	Socket->OnRawMessage().AddLambda([](const void* Data, SIZE_T Size, SIZE_T BytesRemaining) -> void {
		// Log le message brut reçu en tant que chaîne de caractères
		UE_LOG(LogTemp, Display, TEXT("Message brut reçu : %hs"), (const char*)Data);
	});
    
	Socket->OnMessageSent().AddLambda([](const FString& MessageString) -> void {
		// This code is called after we sent a message to the server.
			UE_LOG(LogTemp, Display, TEXT("%s"), *MessageString);
	});
    
	// And we finally connect to the server. 
	Socket->Connect();
}

void UWebSocketHandler::Close()
{
	Socket->Close();
}

void UWebSocketHandler::SendMessage(const FString& Action, const FString& Message)
{
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
	JsonObject->SetStringField("action", Action);
	JsonObject->SetStringField("message", Message);

	FString OutputString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
	if (FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer)) {
		// Envoyer le message JSON via WebSocket
		Socket->Send(OutputString);
		UE_LOG(LogTemp, Display, TEXT("JSON message sent: %s"), *OutputString);
	} else {
		UE_LOG(LogTemp, Error, TEXT("Failed to serialize JSON"));
	}
}

