// Copyright Face Software. All Rights Reserved.


#include "GameMode/LimenGameModeBase.h"

#include "Actors/LimenGameplayManager.h"
#include "GameState/LimenGameStateBase.h"
#include "HUD/LimenBaseHUD.h"
#include "Pawn/LimenCharacterBase.h"
#include "PlayerController/LimenPlayerControllerBase.h"
#include "PlayerState/LimenPlayerStateBase.h"


ALimenGameModeBase::ALimenGameModeBase()
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
}

void ALimenGameModeBase::BeginPlay()
{
	Super::BeginPlay();
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

bool ALimenGameModeBase::SetPause(APlayerController* PC, FCanUnpause CanUnpauseDelegate)
{
	const bool bResult = Super::SetPause(PC, CanUnpauseDelegate);
	if (IsPaused())
	{
		OnPauseStateChanged.Broadcast(true);
	}
	return bResult;
}

bool ALimenGameModeBase::ClearPause()
{
	const bool bResult = Super::ClearPause();

	if (!IsPaused())
	{
		OnPauseStateChanged.Broadcast(false);
	}
	
	return bResult;
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
