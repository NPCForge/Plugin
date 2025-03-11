#include "AIComponent.h"

void UAIComponent::SendMessageToNPC(const FString& ReceiverChecksum, const FString& Content)
{
	if (UMessageManager* MessageManager = GetWorld()->GetSubsystem<UMessageManager>())
	{
		MessageManager->SendMessage(EntityChecksum, ReceiverChecksum, Content);
		TriggerSendMessageEvent(Content);
	}
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
			JsonBody->SetStringField("sender", Message.SenderChecksum);
			JsonBody->SetStringField("message", Message.Content);
			WebSocketHandler->SendMessage("NewMessage", JsonBody);

			bIsBusy = false; 
		}, DelayTime, false);
	}
}

void UAIComponent::HandleMessage(FMessage Message)
{
	if (Message.ReceiverChecksum == EntityChecksum)
	{
		UE_LOG(LogTemp, Log, TEXT("[UAIComponent::HandleMessage]: %s received from %s : %s"), *EntityChecksum, *Message.SenderChecksum, *Message.Content);
		
		DelayResponse(Message);
	}
}
