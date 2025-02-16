// Copyright Face Software. All Rights Reserved.


#include "GameModes/LimenGameMode.h"

#include "..\..\..\LimenPlayers\Public\Characters\LimenPlayerCharacter.h"
#include "GameStates/LimenGameState.h"
#include "HUDs/LimenHUD.h"
#include "LogMacros/LimenLogMacros.h"
#include "PlayerControllers/LimenPlayerController.h"
#include "PlayerStates/LimenPlayerState.h"


ALimenGameMode::ALimenGameMode()
{
	PrimaryActorTick.bCanEverTick = true;

	bGameMenuPausesGame = false;
	bPauseMenuPausesGame = true;
	bNotificationsPauseGame = true;
	PauseRequestCounter = 0;
	
	DefaultPawnClass = ALimenPlayerCharacter::StaticClass();
	PlayerControllerClass = ALimenPlayerController::StaticClass();
	HUDClass = ALimenHUD::StaticClass();
	PlayerStateClass = ALimenPlayerState::StaticClass();
	GameStateClass = ALimenGameState::StaticClass();
}

void ALimenGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);
}

void ALimenGameMode::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);

	if (auto* LimenController = Cast<ALimenPlayerController>(NewPlayer); IsValid(LimenController))
	{
		LimenController->OnPauseRequested.AddDynamic(this, &ThisClass::HandlePauseRequest);
		LimenController->OnUnPauseRequested.AddDynamic(this, &ThisClass::HandleUnpauseRequest);

		auto* PlayerState = LimenController->GetPlayerState<ALimenPlayerState>();
		check(IsValid(PlayerState))
		
		GetLimenGameState()->AddPlayerToPlayerList(PlayerState);	
	}
}

bool ALimenGameMode::SetPause(APlayerController* PC, FCanUnpause CanUnpauseDelegate)
{
	if (!Super::SetPause(PC, CanUnpauseDelegate))
	{
		return false;
	}

	OnWorldPauseStateChanged.Broadcast(true);
	return true;
}

bool ALimenGameMode::ClearPause()
{
	if (!Super::ClearPause())
	{
		return false;
	}

	OnWorldPauseStateChanged.Broadcast(false);
	return true;
}

void ALimenGameMode::HandlePauseRequest(ALimenPlayerControllerBase* Player, const EPauseReason PauseReason)
{
	check(Player)
	LIMEN_LOG(LogLimen, Log, this, "Pause request recieved")
	
	switch (PauseReason)
	{
	case EPauseReason::Undefined:
		return;
		
	case EPauseReason::PauseMenu:
		if (bPauseMenuPausesGame)
		{
			PauseRequestCounter++;
			SetPause(Player);
		}
		break;
		
	case EPauseReason::GameMenu:
		if (bGameMenuPausesGame)
		{
			PauseRequestCounter++;
			SetPause(Player);
		}
		break;
		
	case EPauseReason::Notification:
		if (bNotificationsPauseGame)
		{
			PauseRequestCounter++;
			SetPause(Player);
		}
		break;
		
	default:
		break;
	}
}

void ALimenGameMode::HandleUnpauseRequest(ALimenPlayerControllerBase* Player)
{
	check(Player)
	PauseRequestCounter--;
	if (PauseRequestCounter == 0)
	{
		Player->SetPause(false);
	}
}
