#include "AIComponent.h"

void UAIComponent::SendMessageToNPC(const FString& ReceiverChecksum, const FString& Content)
{
	if (UMessageManager* MessageManager = GetWorld()->GetSubsystem<UMessageManager>())
	{
		MessageManager->SendMessage(EntityChecksum, ReceiverChecksum, Content);
	}
}

TArray<FMessage> UAIComponent::GetReceivedMessages() const
{
	if (UMessageManager* MessageManager = GetWorld()->GetSubsystem<UMessageManager>())
	{
		return MessageManager->GetMessagesForNPC(EntityChecksum);
	}
	return TArray<FMessage>();
}

void UAIComponent::HandleMessage(FMessage Message)
{
	if (Message.ReceiverChecksum == EntityChecksum)
	{
		UE_LOG(LogTemp, Display, TEXT("[UAIComponent::HandleMessage]: %s received from %s : %s"), *EntityChecksum, *Message.SenderChecksum, *Message.Content);
		// WebSocketHandler->SendMessage("TakeDecision", *Message.Content);
	}
}
