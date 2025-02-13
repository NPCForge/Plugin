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
		TSharedPtr<FJsonObject> JsonBody = MakeShareable(new FJsonObject());
		JsonBody->SetStringField("message", Message.Content);
		WebSocketHandler->SendMessage("TalkTo", JsonBody);

		// its not just about that, we must also send the checksum of the sender and the receiver so the AI says who it wanna talk to.
		// We must add in the system prompt (currently UE AI prompt) the fact that the AI should says who to talk to.
	}
}
