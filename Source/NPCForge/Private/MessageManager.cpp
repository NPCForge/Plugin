#include "MessageManager.h"

UMessageManager::UMessageManager()
{
}

void UMessageManager::SendMessage(const FString& SenderID, const FString& ReceiverID, const FString& Content, TArray<FString>& ReceiversNames)
{
	const FMessage NewMessage(SenderID, ReceiverID, Content, ReceiversNames);
	MessageLog.Add(NewMessage);

	UE_LOG(LogTemp, Log, TEXT("[UMessageManager::SendMessage]: Sending message: Sender=%s, Receiver=%s, Content=%s"), *NewMessage.SenderChecksum, *NewMessage.ReceiverChecksum, *NewMessage.Content);

	NewMessageReceivedEvent.Broadcast(NewMessage);
}

TArray<FMessage> UMessageManager::GetAllMessages() const
{
	return MessageLog;
}

TArray<FMessage> UMessageManager::GetMessagesForNPC(const FString& NPCChecksum) const
{
	TArray<FMessage> FilteredMessages;
	for (const FMessage& Message : MessageLog)
	{
		if (Message.ReceiverChecksum == NPCChecksum)
		{
			FilteredMessages.Add(Message);
		}
	}
	return FilteredMessages;
}


