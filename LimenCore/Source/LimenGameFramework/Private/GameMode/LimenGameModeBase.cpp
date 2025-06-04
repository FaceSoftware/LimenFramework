// Copyright Face Software. All Rights Reserved.


#include "GameMode/LimenGameModeBase.h"

#include "Actors/LimenGameplayManager.h"
#include "BlueprintLibraries/LimenCoreStatics.h"
#include "GameState/LimenGameStateBase.h"
#include "HUD/LimenBaseHUD.h"
#include "Pawn/LimenCharacterBase.h"
#include "PlayerController/LimenPlayerControllerBase.h"
#include "PlayerState/LimenPlayerStateBase.h"


ALimenGameModeBase::ALimenGameModeBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bTickEvenWhenPaused = true;

	bUseSeamlessTravel = true;
	DefaultPawnClass = ALimenCharacterBase::StaticClass();
	PlayerControllerClass = ALimenPlayerControllerBase::StaticClass();
	HUDClass = ALimenBaseHUD::StaticClass();
	PlayerStateClass = ALimenPlayerStateBase::StaticClass();
	GameStateClass = ALimenGameStateBase::StaticClass();
}

void ALimenGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);
	SpawnManagers();
	// OverrideLimenClasses();

	if (!ErrorMessage.IsEmpty())
	{
		ULimenCoreStatics::LimenLog(this, ErrorMessage, ELogType::Error);
	}
	else
	{
		ULimenCoreStatics::LimenLog(this, TEXT("Game initialized successfully"), ELogType::Log, false);
	}
}

void ALimenGameModeBase::BeginPlay()
{
	Super::BeginPlay();
}

void ALimenGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (GetLimenGameState())
	{
		LimenGameState->AddPlayerToPlayerList(NewPlayer->PlayerState.Get());
	}
}

void ALimenGameModeBase::Logout(AController* Exiting)
{
	const APlayerController* PlayerController = Cast<APlayerController>(Exiting);
	if (PlayerController && GetLimenGameState())
	{
		LimenGameState->RemovePlayerFromPlayerList(PlayerController->PlayerState.Get());
	}
	
	Super::Logout(Exiting);
}

void ALimenGameModeBase::InitSeamlessTravelPlayer(AController* NewController)
{
	Super::InitSeamlessTravelPlayer(NewController);

	const APlayerController* PlayerController = Cast<APlayerController>(NewController);
	if (GetLimenGameState() && PlayerController)
	{
		LimenGameState->AddPlayerToPlayerList(PlayerController->PlayerState.Get());
	}
}

bool ALimenGameModeBase::SetPause(APlayerController* PC, FCanUnpause CanUnpauseDelegate)
{
	if (!Super::SetPause(PC, CanUnpauseDelegate)) return false;

	OnGamePauseStateChanged.Broadcast(true);
	return true;
}

bool ALimenGameModeBase::ClearPause()
{
	if (!Super::ClearPause()) return false;

	OnGamePauseStateChanged.Broadcast(false);
	return true;
}

ALimenGameStateBase* ALimenGameModeBase::GetLimenGameState()
{
	LimenGameState = GetGameState<ALimenGameStateBase>();
	check(LimenGameState)
	
	return LimenGameState.Get();
}

void ALimenGameModeBase::ResetManagers()
{
	for (const TObjectPtr<ALimenGameplayManager>& Manager : ManagersList)
	{
		Manager->Reset();
	}
}

void ALimenGameModeBase::SpawnManagers()
{
	for (auto& ManagerSoftClassPtr : ManagersClassList)
	{
		checkf(!ManagerSoftClassPtr.IsNull(), TEXT("Invalid manager class"))

		auto* ManagerClass = ManagerSoftClassPtr.LoadSynchronous();
		check(IsValid(ManagerClass))

		auto* ManagerInstance = GetWorld()->SpawnActor<ALimenGameplayManager>(ManagerClass);
		checkf(IsValid(ManagerInstance), TEXT("Failed to spawn Manager of class %s"), *ManagerClass->GetName())
		
		ManagerInstance->BindGameMode(this);
		ManagersList.Push(TObjectPtr<ALimenGameplayManager>(ManagerInstance));
	}
}
