// Copyright Face Software. All Rights Reserved.


#include "GameModes/LimenGameModeBase.h"

#include "Actors/LimenGameplayManager.h"
#include "GameStates/LimenGameState.h"
#include "HUDs/LimenBaseHUD.h"
#include "Characters/LimenPlayerCharacter.h"
#include "PlayerControllers/LimenPlayerControllerBase.h"
#include "PlayerStates/LimenBasePlayerState.h"

ALimenGameModeBase::ALimenGameModeBase()
{
	PrimaryActorTick.bTickEvenWhenPaused = true;

	bUseSeamlessTravel = true;
	DefaultPawnClass = ALimenPlayerCharacter::StaticClass();
	PlayerControllerClass = ALimenPlayerControllerBase::StaticClass();
	HUDClass = ALimenBaseHUD::StaticClass();
	PlayerStateClass = ALimenBasePlayerState::StaticClass();
	GameStateClass = ALimenGameState::StaticClass();
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

ALimenGameState* ALimenGameModeBase::GetLimenGameState()
{
	LimenGameState = GetGameState<ALimenGameState>();
	check(LimenGameState)
	
	return LimenGameState.Get();
}

void ALimenGameModeBase::ResetManagers()
{
	for (TObjectPtr<ALimenGameplayManager>& Manager : ManagersList)
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
