﻿#include "AIComponent.h"

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
		// Subscribe to message reception event
		MessageManager->NewMessageReceivedEvent.AddDynamic(this, &UAIComponent::HandleMessage);
	}

	if (!bIsRegistered) {
		WebSocketHandler->RegisterAPI(EntityChecksum, UniqueName, PersonalityPrompt);
	} else {
		WebSocketHandler->ConnectAPI(EntityChecksum);
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

	// Vérifier si l'owner possède déjà un AIController
	if (!OwnerPawn->GetController())
	{
		// Spawner et posséder un AIController
		AAIController* NewAIController = GetWorld()->SpawnActor<AAIController>(AAIController::StaticClass());
		if (NewAIController)
		{
			NewAIController->Possess(OwnerPawn);
			UE_LOG(LogTemp, Warning, TEXT("AIController ajouté dynamiquement à %s"), *OwnerPawn->GetName());
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Impossible de créer l'AIController"));
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
		if (bIsConnected && !bIsBusy && EntityChecksum == "3fc289f3001056ad01357a00efd76ac3")
		{
			const FString EnvironmentPrompt = ScanEnvironment();
			
			TakeDecision(EnvironmentPrompt);

			bIsBusy = true;
		}
	}
}
