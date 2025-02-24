#include "AIComponent.h"

void UAIComponent::SendMessageToNPC(const FString& ReceiverChecksum, const FString& Content)
{
	if (UMessageManager* MessageManager = GetWorld()->GetSubsystem<UMessageManager>())
	{
		MessageManager->SendMessage(EntityChecksum, ReceiverChecksum, Content);
		TriggerSendMessageEvent(Content);
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

void UAIComponent::DelayResponse(FMessage Message)
{
	int MessageLength = Message.Content.Len();
	float DelayTime = FMath::Clamp(MessageLength * 0.1f, 1.0f, 5.0f);

	if (GetWorld())
	{
		FTimerManager& TimerManager = GetWorld()->GetTimerManager();

		TimerManager.SetTimer(ResponseTimerHandle, [this, Message]()
		{
			TSharedPtr<FJsonObject> JsonBody = MakeShareable(new FJsonObject());
			JsonBody->SetStringField("interlocutor", Message.SenderChecksum);
			JsonBody->SetStringField("message", Message.Content);
			WebSocketHandler->SendMessage("TalkTo", JsonBody);
		}, DelayTime, false);
	}
}

void UAIComponent::HandleMessage(FMessage Message)
{
	if (Message.ReceiverChecksum == EntityChecksum)
	{
		UE_LOG(LogTemp, Display, TEXT("[UAIComponent::HandleMessage]: %s received from %s : %s"), *EntityChecksum, *Message.SenderChecksum, *Message.Content);

		DelayResponse(Message);
	}
}
