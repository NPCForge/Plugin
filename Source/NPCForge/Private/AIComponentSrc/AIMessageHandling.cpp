#include "AIComponent.h"

void UAIComponent::SendMessageToNPC(const FString& ReceiverID, const FString& Content)
{
	if (UMessageManager* MessageManager = GetWorld()->GetSubsystem<UMessageManager>())
	{
		MessageManager->SendMessage(UniqueID, ReceiverID, Content);
	}
}

TArray<FMessage> UAIComponent::GetReceivedMessages() const
{
	if (UMessageManager* MessageManager = GetWorld()->GetSubsystem<UMessageManager>())
	{
		return MessageManager->GetMessagesForNPC(UniqueID);
	}
	return TArray<FMessage>();
}

void UAIComponent::HandleMessage(FMessage Message)
{
	UE_LOG(LogTemp, Display, TEXT("FROM AI: Message reçu de %s : %s"), *Message.SenderID, *Message.Content);
	// WebSocketHandler->SendMessage("TakeDecision", Message.Content);
}
