#include "MessageManager.h"

UMessageManager::UMessageManager()
{
}

void UMessageManager::SendMessage(const FString& SenderID, const FString& ReceiverID, const FString& Content)
{
	FMessage NewMessage(SenderID, ReceiverID, Content);
	MessageLog.Add(NewMessage);

	UE_LOG(LogTemp, Log, TEXT("[UMessageManager::SendMessage]: Sending message: Sender=%s, Receiver=%s, Content=%s"), *NewMessage.SenderChecksum, *NewMessage.ReceiverChecksum, *NewMessage.Content);

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
		if (Message.ReceiverChecksum == NPCID)
		{
			FilteredMessages.Add(Message);
		}
	}
	return FilteredMessages;
}

void UMessageManager::TriggerCustomEvent()
{
	UE_LOG(LogTemp, Log, TEXT("[UMessageManager::TriggerCustomEvent]: TriggerCustomEvent called in UMessageManager"));
}

