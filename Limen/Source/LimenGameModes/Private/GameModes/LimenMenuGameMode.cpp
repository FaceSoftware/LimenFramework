// Copyright Face Software. All Rights Reserved.


#include "GameModes/LimenMenuGameMode.h"

#include "PlayerControllers/LimenMenuPlayerController.h"
#include "SaveData/LimenGameSaveData.h"
#include "Subsystems/LimenGameSaveSubsystem.h"
#include "Subsystems/LimenLevelManagerSubsystem.h"


ALimenMenuGameMode::ALimenMenuGameMode()
{
	bUseSeamlessTravel = false;
}

void ALimenMenuGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	ALimenMenuPlayerController* LimenPlayer = Cast<ALimenMenuPlayerController>(NewPlayer);
	if (LimenPlayer == nullptr)
	{
		return;
	}

	LimenPlayer->OnNewGameRequested.AddUObject(this, &ThisClass::StartNewGame);
	LimenPlayer->OnLoadGameRequested.AddUObject(this, &ThisClass::StartLoadingGame);
}

void ALimenMenuGameMode::StartNewGame(APlayerController* Player)
{
	ULimenLevelManagerSubsystem* LevelManager = GetWorld()->GetGameInstance()->GetSubsystem<ULimenLevelManagerSubsystem>();
	check(LevelManager != nullptr);
	
	LevelManager->OpenGameLevel(0);
}

void ALimenMenuGameMode::StartLoadingGame(APlayerController* Player, const ULimenSaveData* SaveGame)
{
	const ULimenGameSaveData* GameSaveData = Cast<ULimenGameSaveData>(SaveGame);
	check(GameSaveData != nullptr);

	ULimenLevelManagerSubsystem* LevelManager = GetWorld()->GetGameInstance()->GetSubsystem<ULimenLevelManagerSubsystem>();
	check(LevelManager != nullptr);

	ULimenGameSaveSubsystem* GameSaveSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<ULimenGameSaveSubsystem>();
	check(GameSaveSubsystem != nullptr);

	GameSaveSubsystem->ScheduleGameLoadOnMapChange();
	LevelManager->OpenGameLevel(GameSaveData->GameLevelIndex);
}
