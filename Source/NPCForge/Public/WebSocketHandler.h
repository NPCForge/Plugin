// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "WebSocketsModule.h"
#include "IWebSocket.h"

#include "WebSocketHandler.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWebSocketMessageReceived, const FString&, Message);

UCLASS()
class NPCFORGE_API UWebSocketHandler : public UObject
{
	GENERATED_BODY()

public:
	void Initialize();
	void Close();
	void SendMessage(const FString& Action, TSharedPtr<FJsonObject> JsonBody);
	void ConnectAPI(const FString& Checksum);
	void RegisterAPI(const FString& Checksum, const FString& Name, const FString& Prompt);
	void DisconnectAPI();

	void SetToken(const FString& Token);

	TSet<FString> MessagesSent;

	UPROPERTY(BlueprintAssignable, Category = "WebSocket")
	FOnWebSocketMessageReceived OnMessageReceived;

private:
	const FString ServerURL = TEXT("ws://127.0.0.1:3000/ws");
	const FString ServerProtocol = TEXT("ws");
	FString Token = TEXT("");

	TSharedPtr<IWebSocket> Socket = FWebSocketsModule::Get().CreateWebSocket(ServerURL, ServerProtocol);
};
