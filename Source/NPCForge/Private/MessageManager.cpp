#include "MessageManager.h"

UMessageManager::UMessageManager()
{
}

void UMessageManager::SendMessage(const FString& SenderID, const FString& ReceiverID, const FString& Content)
{
	FMessage NewMessage(SenderID, ReceiverID, Content);
	MessageLog.Add(NewMessage);

	UE_LOG(LogTemp, Display, TEXT("[NPCForge:MessageManager]: Sending message: Sender=%s, Receiver=%s, Content=%s"), *NewMessage.SenderID, *NewMessage.ReceiverID, *NewMessage.Content);

	NewMessageReceivedEvent.Broadcast(NewMessage);
}

TArray<FMessage> UMessageManager::GetAllMessages() const
{
	return MessageLog;
}

TArray<FMessage> UMessageManager::GetMessagesForNPC(const FString& NPCID) const
{
	TArray<FMessage> FilteredMessages;
	for (const FMessage& Message : MessageLog)
	{
		if (Message.ReceiverID == NPCID)
		{
			FilteredMessages.Add(Message);
		}
	}
	return FilteredMessages;
}

void UMessageManager::TriggerCustomEvent()
{
	UE_LOG(LogTemp, Log, TEXT("[NPCForge:MessageManager]: TriggerCustomEvent called in UMessageManager"));
}

