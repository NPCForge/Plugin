// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "WebSocketsModule.h"
#include "IWebSocket.h"

#include "WebSocketHandler.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWebSocketMessageReceived, const FString&, Message);

UCLASS(Blueprintable)
class NPCFORGE_API UWebSocketHandler : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "WebSocket")
	void Initialize(bool IsEntity);

	UFUNCTION(BlueprintCallable, Category = "WebSocket")
	void Close();
	
	void SendMessage(const FString& Action, TSharedPtr<FJsonObject> JsonBody);
	void ConnectAPI(const FString& Checksum);
	void RegisterAPI(const FString& Checksum, const FString& Name, const FString& Prompt);
	void DisconnectAPI();

	UFUNCTION(BlueprintCallable, Category = "WebSocket")
	void ResetGame();

	void SendResetMessage();

	void SetToken(const FString& Token);

	UPROPERTY(BlueprintAssignable, Category = "WebSocket")
	FOnWebSocketMessageReceived OnMessageReceived;

	FTimerHandle ResetGameTimerHandle;

private:
	const FString ServerURL = TEXT("ws://127.0.0.1:3000/ws");
	const FString ServerProtocol = TEXT("ws");
	FString Token = TEXT("");
	bool bIsEntity = true;

	TSharedPtr<IWebSocket> Socket = FWebSocketsModule::Get().CreateWebSocket(ServerURL, ServerProtocol);
};
