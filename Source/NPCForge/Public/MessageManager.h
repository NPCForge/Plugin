// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "MessageManager.generated.h"

USTRUCT(BlueprintType)
struct FMessage
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FString SenderChecksum;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FString ReceiverChecksum;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FString Content;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TArray<FString> ReceiversNames;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FDateTime Timestamp;

    FMessage() : SenderChecksum(""), ReceiverChecksum(""), Content(""), Timestamp(FDateTime::Now()) {}
    FMessage(FString InSenderChecksum, FString InReceiverChecksum, FString InContent, TArray<FString> InReceiversNames)
        : SenderChecksum(InSenderChecksum), ReceiverChecksum(InReceiverChecksum), Content(InContent),
        ReceiversNames(InReceiversNames), Timestamp(FDateTime::Now()) {}
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAIMessageReceivedEvent, FMessage, Message);

UCLASS()
class NPCFORGE_API UMessageManager : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    UMessageManager();

    // Send message
    UFUNCTION(BlueprintCallable)
    void SendMessage(const FString& SenderID, const FString& ReceiverID, const FString& Content, TArray<FString>& ReceiversNames);

    // Get all messages realtime
    UFUNCTION(BlueprintCallable)
    TArray<FMessage> GetAllMessages() const;

    // Get messages for specific NPC
    UFUNCTION(BlueprintCallable)
    TArray<FMessage> GetMessagesForNPC(const FString& NPCChecksum) const;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnAIMessageReceivedEvent NewMessageReceivedEvent;

private:
    TArray<FMessage> MessageLog;
};
