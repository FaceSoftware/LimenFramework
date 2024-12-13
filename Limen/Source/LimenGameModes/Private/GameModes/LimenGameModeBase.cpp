// Copyright Face Software. All Rights Reserved.


#include "GameModes/LimenGameModeBase.h"

#include "Actors/LimenGameplayManager.h"
#include "Characters/LimenCharacterBase.h"
#include "GameStates/LimenGameStateBase.h"
#include "HUDs/LimenBaseHUD.h"
#include "PlayerControllers/LimenPlayerControllerBase.h"
#include "PlayerStates/LimenBasePlayerState.h"


ALimenGameModeBase::ALimenGameModeBase()
{
	PrimaryActorTick.bTickEvenWhenPaused = true;

	bUseSeamlessTravel = true;
	DefaultPawnClass = ALimenCharacterBase::StaticClass();
	PlayerControllerClass = ALimenPlayerControllerBase::StaticClass();
	HUDClass = ALimenBaseHUD::StaticClass();
	PlayerStateClass = ALimenBasePlayerState::StaticClass();
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
