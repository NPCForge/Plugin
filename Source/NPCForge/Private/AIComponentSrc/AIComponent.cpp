#include "AIComponent.h"

UAIComponent::UAIComponent() {
  GameMode = nullptr;
  WebSocketHandler = nullptr;
  PrimaryComponentTick.bCanEverTick = true;
}

void UAIComponent::BeginPlay() {
  Super::BeginPlay();

  UE_LOG(LogTemp, Log, TEXT("[UAIComponent::BeginPlay]: %s joined the game!"),
         *UniqueName);

  if (const UWorld *World = GetOwner()->GetWorld()) {
    if (auto *MyGI = Cast<UNPCForgeGameInstance>(World->GetGameInstance())) {
      if (!WebSocketHandler) {
        WebSocketHandler = MyGI->GetWebSocketHandler();
        WebSocketHandler->OnMessageReceived.AddDynamic(
            this, &UAIComponent::HandleWebSocketMessage);
        WebSocketHandler->OnReady.AddDynamic(this,
                                             &UAIComponent::OnWebsocketReady);
      }
    } else {
      UE_LOG(LogTemp, Warning, TEXT("Not Found Game Instance"));
    }
  }

  GameMode = Cast<AMyGameMode>(GetWorld()->GetAuthGameMode());
}

void UAIComponent::CheckGameRole() {
  RoleCheckElapsed += 0.2f;

  if (AActor *Owner = GetOwner();
      Owner &&
      Owner->GetClass()->ImplementsInterface(UAIInterface::StaticClass())) {
    if (FString Role = IAIInterface::Execute_GetGameRole(Owner);
        Role != "None") {
      CachedRole = Role;
      UE_LOG(LogTemp, Warning, TEXT("Final Role: %s"), *Role);
      GetWorld()->GetTimerManager().ClearTimer(RoleCheckTimerHandle);

      const FString CombinedString =
          FString::Printf(TEXT("%s%s%d"), *UniqueName, *PersonalityPrompt,
                          WebSocketHandler->ApiUserID);
      EntityChecksum = FMD5::HashAnsiString(*CombinedString);

      if (!WebSocketHandler->IsEntityRegistered(EntityChecksum)) {
        WebSocketHandler->RegisterEntityOnApi(UniqueName, PersonalityPrompt,
                                              EntityChecksum, CachedRole);
        UE_LOG(LogTemp, Log, TEXT("Entity registered with role: %s"), *Role);
      }
      bIsWebsocketConnected = true;

      return;
    }
  }

  if (RoleCheckElapsed >= 10.0f) {
    UE_LOG(LogTemp, Error, TEXT("Timeout waiting for GetGameRole() != None"));
    GetWorld()->GetTimerManager().ClearTimer(RoleCheckTimerHandle);
  }
}

void UAIComponent::OnWebsocketReady() {
  RoleCheckElapsed = 0.0f;

  GetWorld()->GetTimerManager().SetTimer(
      RoleCheckTimerHandle, this, &UAIComponent::CheckGameRole, 0.2f, true);
}

void UAIComponent::TriggerSendMessageEvent(const FString Message,
                                           const FString Reasoning) const {
  if (OnSendMessage.IsBound()) {
    OnSendMessage.Broadcast(Message, Reasoning);
  }
}

void UAIComponent::EndPlay(const EEndPlayReason::Type EndPlayReason) {
  Super::EndPlay(EndPlayReason);
}

void UAIComponent::TickComponent(
    const float DeltaTime, const ELevelTick TickType,
    FActorComponentTickFunction *ThisTickFunction) {
  Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

  if (!GetOwner())
    return;

  FString CurrentPhase = GameMode ? GameMode->GetPhase() : FString();
  if (CurrentPhase != LastKnownPhase) {
    bHasVotedInCurrentPhase = false;
    bIsBusy = false;
    LastKnownPhase = CurrentPhase;
  }
  
  TimeSinceLastDecision += DeltaTime;

  if (bIsWebsocketConnected && !bIsBusy &&
      TimeSinceLastDecision >= DecisionInterval && CachedRole != "None") {
    bIsBusy = true;
    TimeSinceLastDecision = 0.0f;

    const FString EnvironmentPrompt = ScanEnvironment();

    if (CurrentPhase == TEXT("Voting") && bHasVotedInCurrentPhase) {
      return;
    }
    if (CurrentPhase == TEXT("Night") && CachedRole != "Werewolf")
    {
      return;
    }

    UE_LOG(LogTemp, Log, TEXT("Env = %s"), *EnvironmentPrompt)

    MakeDecision(EnvironmentPrompt);
  }
}
