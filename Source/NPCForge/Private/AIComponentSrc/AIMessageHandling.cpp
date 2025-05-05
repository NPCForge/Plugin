#include "AIComponent.h"

void UAIComponent::SendMessageToNPC(const FString& ReceiverChecksum, const FString& Content, TArray<FString>& ReceiversNames)
{
	if (UMessageManager* MessageManager = GetWorld()->GetSubsystem<UMessageManager>())
	{
		MessageManager->SendMessage(EntityChecksum, ReceiverChecksum, Content, ReceiversNames);
		TriggerSendMessageEvent(Content);
	}
}

void UAIComponent::DelayBusy()
{
	if (GetWorld())
	{
		FTimerManager& TimerManager = GetWorld()->GetTimerManager();

		TimerManager.SetTimer(ResponseTimerHandle, [this]()
		{
			bIsBusy = false; 
		}, 5.0f, false);
	}
}


void UAIComponent::DelayResponse(FMessage Message)
{
	const int MessageLength = Message.Content.Len();
	const float DelayTime = FMath::Clamp(MessageLength * 0.1f, 1.0f, 5.0f);

	if (GetWorld())
	{
		FTimerManager& TimerManager = GetWorld()->GetTimerManager();

		TimerManager.SetTimer(ResponseTimerHandle, [this, Message]()
		{
			const FString CombinedNames = FString::Join(Message.ReceiversNames, TEXT(" "));
			const FString CombinedString = FString::Printf(TEXT("%s%s%s"), *Message.SenderChecksum, *Message.Content, *CombinedNames);
			const FString MessageChecksum = FMD5::HashAnsiString(*CombinedString);

			if (!WebSocketHandler->MessagesSent.Contains(MessageChecksum))
			{
				WebSocketHandler->MessagesSent.Add(MessageChecksum);
				UE_LOG(LogTemp, Log, TEXT("[UAIComponent::DelayResponse]: Successfully added message: %s"), *MessageChecksum);

				const TSharedPtr<FJsonObject> JsonBody = MakeShareable(new FJsonObject());

				TArray<TSharedPtr<FJsonValue>> JsonArray;
				for (const FString& Receiver : Message.ReceiversNames)
				{
					// Need to send receiver checksum, not name
					JsonArray.Add(MakeShared<FJsonValueString>(Receiver));
				}
				
				JsonBody->SetStringField("sender", Message.SenderChecksum);
				JsonBody->SetArrayField("receivers", JsonArray);
				JsonBody->SetStringField("message", Message.Content);
				WebSocketHandler->SendMessage("NewMessage", JsonBody);
			}
			
			DelayBusy();
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
