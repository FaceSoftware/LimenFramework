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

	for (auto& StreamingLevel : GetWorld()->GetStreamingLevels())
	{
		if (StreamingLevel->IsStreamingStatePending())
		{
			InitialStreamingLevels++;
			StreamingLevel->OnLevelShown.AddUniqueDynamic(this, &ThisClass::InitialStreamingLevelShown);
			InitialStreamingLevelsList.Push(StreamingLevel);
		}
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

void ALimenGameModeBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	if (GameState)
	{
		TickGameState(GameState, DeltaSeconds);
	}
}

ALimenGameStateBase* ALimenGameModeBase::GetLimenGameState()
{
	LimenGameState = GetGameState<ALimenGameStateBase>();
	check(LimenGameState.IsValid())
	
	return LimenGameState.Get();
}

void ALimenGameModeBase::ResetManagers()
{
	for (const TWeakObjectPtr<ALimenGameplayManager>& Manager : ManagersList)
	{
		Manager->Reset();
	}
}

bool ALimenGameModeBase::IsInitialLevelStreamingComplete() const
{
	return InitialStreamingLevelsList.IsEmpty();
}

void ALimenGameModeBase::InitialLevelStreamingComplete()
{
}

void ALimenGameModeBase::TickGameState(AGameStateBase* InGameState, float DeltaSeconds)
{
	for (auto& Player : InGameState->PlayerArray)
	{
		TickPlayerState(Player.Get(), DeltaSeconds);
	}
}

void ALimenGameModeBase::TickPlayerState(APlayerState* InPlayerState, float DeltaSeconds)
{
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

void ALimenGameModeBase::InitialStreamingLevelShown()
{
	InitialStreamingLevels--;
	check(InitialStreamingLevels >= 0)
	if (InitialStreamingLevels == 0)
	{
		for (auto& StreamingLevel : InitialStreamingLevelsList)
		{
			if (StreamingLevel.IsValid())
			{
				StreamingLevel->OnLevelShown.RemoveDynamic(this, &ThisClass::InitialStreamingLevelShown);
			}
		}
		InitialStreamingLevelsList.Empty(0);

		InitialLevelStreamingComplete();
	}
}
