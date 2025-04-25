// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "WebSocketsModule.h"
#include "IWebSocket.h"
#include "SaveEntityState.h"
#include "Kismet/GameplayStatics.h"

#include "WebSocketHandler.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWebSocketMessageReceived, const FString&, Message);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWebSocketReady);

UCLASS(Blueprintable)
class NPCFORGE_API UWebSocketHandler : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "WebSocket")
	void Initialize();

	UFUNCTION(BlueprintCallable, Category = "WebSocket")
	void Close();
	
	void SendMessage(const FString& Action, TSharedPtr<FJsonObject> JsonBody);

	void ConnectAPI();
	void RegisterAPI();
	void DisconnectAPI();
	
	void RegisterEntity(const FString& Checksum, const FString& ID) const;
	void RegisterEntityOnApi(const FString &Name, const FString &Prompt, const FString &Checksum);

	UFUNCTION(BlueprintCallable, Category = "WebSocket")
	void ResetGame();

	void SendResetMessage();

	void SetToken(const FString& Token);

	void SaveInstanceState() const;
	void LoadInstanceState();

	void HandleReceivedMessage(const FString &Message);

	bool IsEntityRegistered(const FString &Checksum) const;

	TSet<FString> MessagesSent;

	UPROPERTY(BlueprintAssignable, Category = "WebSocket")
	FOnWebSocketMessageReceived OnMessageReceived;

	UPROPERTY(BlueprintAssignable, Category = "WebSocket")
	FOnWebSocketReady OnReady;

	FTimerHandle ResetGameTimerHandle;

	bool bIsRegistered = false;
	bool bIsConnected = false;

	int ApiUserID = -1;
private:
	const FString ServerURL = TEXT("ws://127.0.0.1:3000/ws");
	const FString ServerProtocol = TEXT("ws");
	FString Token = TEXT("");

	TSharedPtr<TMap<FString, FString>> RegisteredEntities = MakeShareable(new TMap<FString, FString>());

	TSharedPtr<IWebSocket> Socket = FWebSocketsModule::Get().CreateWebSocket(ServerURL, ServerProtocol);
};
