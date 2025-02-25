#include "AIComponent.h"

UAIComponent::UAIComponent()
{
	WebSocketHandler = nullptr;
	PrimaryComponentTick.bCanEverTick = true;
}

void UAIComponent::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Log, TEXT("[UAIComponent::BeginPlay]: %s joined the game!"), *UniqueName);

	if (!WebSocketHandler)
	{
		WebSocketHandler = NewObject<UWebSocketHandler>(this);
		if (WebSocketHandler)
		{
			WebSocketHandler->Initialize();
			WebSocketHandler->OnMessageReceived.AddDynamic(this, &UAIComponent::HandleWebSocketMessage);
		}
	}
	
	// Generate checksum from name + prompt
	const FString CombinedString = FString::Printf(TEXT("%s%s"), *UniqueName, *PersonalityPrompt);
	EntityChecksum = FMD5::HashAnsiString(*CombinedString);
	
	LoadEntityState();
	
	if (UMessageManager* MessageManager = GetWorld()->GetSubsystem<UMessageManager>())
	{
		MessageManager->NewMessageReceivedEvent.AddDynamic(this, &UAIComponent::HandleMessage);
	}

	if (!bIsRegistered) {
		WebSocketHandler->RegisterAPI(EntityChecksum, UniqueName, PersonalityPrompt);
	} else {
		WebSocketHandler->ConnectAPI(EntityChecksum);
	}
}

void UAIComponent::TriggerSendMessageEvent(FString Message)
{
	if (OnSendMessage.IsBound())
	{
		OnSendMessage.Broadcast(Message);
	}
}

void UAIComponent::AddAIController()
{
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("UMyCustomComponent: Owner is not a Pawn!"));
		return;
	}

	if (!OwnerPawn->GetController())
	{
		AAIController* NewAIController = GetWorld()->SpawnActor<AAIController>(AAIController::StaticClass());
		if (NewAIController)
		{
			NewAIController->Possess(OwnerPawn);
			UE_LOG(LogTemp, Warning, TEXT("AIController dynamically added to %s"), *OwnerPawn->GetName());
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Unable to create AIController"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("%s a déjà un AIController"), *OwnerPawn->GetName());
	}
}


void UAIComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	WebSocketHandler->Close();
	SaveEntityState();
	
	Super::EndPlay(EndPlayReason);
}

void UAIComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	// Placeholder for functionality that runs each tick.
	if (GetOwner())
	{
		if (bIsConnected && !bIsBusy && EntityChecksum == "b6767fbc10c2d571e190f82315706017")
		{
			const FString EnvironmentPrompt = ScanEnvironment();
			
			TakeDecision(EnvironmentPrompt);

			bIsBusy = true;
		}
	}
}
