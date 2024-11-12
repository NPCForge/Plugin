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
    FString SenderID;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FString ReceiverID;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FString Content;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FDateTime Timestamp;

    FMessage() : SenderID(""), ReceiverID(""), Content(""), Timestamp(FDateTime::Now()) {}
    FMessage(FString InSenderID, FString InReceiverID, FString InContent)
        : SenderID(InSenderID), ReceiverID(InReceiverID), Content(InContent), Timestamp(FDateTime::Now()) {}
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAIMessageReceivedEvent, FMessage, Message);

UCLASS()
class NPCFORGE_API UMessageManager : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    UMessageManager();

    // Envoi d'un message
    UFUNCTION(BlueprintCallable)
    void SendMessage(const FString& SenderID, const FString& ReceiverID, const FString& Content);

    // Récupération de tous les messages en temps réel
    UFUNCTION(BlueprintCallable)
    TArray<FMessage> GetAllMessages() const;

    // Récupération de messages pour un NPC spécifique
    UFUNCTION(BlueprintCallable)
    TArray<FMessage> GetMessagesForNPC(const FString& NPCID) const;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnAIMessageReceivedEvent NewMessageReceivedEvent;

    // Fonction pour déclencher l'événement
    UFUNCTION(BlueprintCallable, Category = "Events")
    void TriggerCustomEvent();

private:
    TArray<FMessage> MessageLog;
};
