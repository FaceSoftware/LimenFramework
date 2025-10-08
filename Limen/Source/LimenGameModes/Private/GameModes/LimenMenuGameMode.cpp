// Copyright Face Software. All Rights Reserved.


#include "GameModes/LimenMenuGameMode.h"

#include "Engine/GameInstance.h"
#include "PlayerControllers/LimenMenuPlayerController.h"
#include "SaveData/LimenGameSaveData.h"
#include "Subsystems/LimenGameSaveSubsystem.h"
#include "Subsystems/LimenLevelManagerSubsystem.h"
#include "Subsystems/LimenModalsSubsystem.h"


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
	
	LevelManager->OpenGameLevel(ELevelOpenContext::Local, 0);
}

void ALimenMenuGameMode::StartLoadingGame(APlayerController* Player, const ULimenSaveData* SaveGame)
{
	const ULimenGameSaveData* GameSaveData = Cast<ULimenGameSaveData>(SaveGame);
	check(GameSaveData != nullptr);

	ULimenLevelManagerSubsystem* LevelManager = GetWorld()->GetGameInstance()->GetSubsystem<
		ULimenLevelManagerSubsystem>();
	check(LevelManager != nullptr);

	ULimenGameSaveSubsystem* GameSaveSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<ULimenGameSaveSubsystem>();
	check(GameSaveSubsystem != nullptr);

	if (LevelManager->IsGameLevelIndexValid(GameSaveData->GameLevelIndex))
	{
		GameSaveSubsystem->ScheduleGameLoadOnMapChange();
		LevelManager->OpenGameLevel(ELevelOpenContext::Local, GameSaveData->GameLevelIndex);
		return;
	}

	ULimenModalsSubsystem* ModalsSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<ULimenModalsSubsystem>();
	check(ModalsSubsystem != nullptr);

	ULimenGenericModalWidget* Modal = ModalsSubsystem->DisplayConfirmationModal(FModalParams(TEXT("Save Data Corrupted"), TEXT("Something went wrong loading the save data.")));
	check(Modal != nullptr)
	Modal->OnModalResponseReceived.AddDynamic(this, &ThisClass::DataCorruptedModalResponse);
}

void ALimenMenuGameMode::DataCorruptedModalResponse(ULimenGenericModalWidget* ModalWidget, bool bAccepted)
{
	ModalWidget->OnModalResponseReceived.RemoveDynamic(this, &ThisClass::DataCorruptedModalResponse);

	ULimenLevelManagerSubsystem* LevelManager = GetWorld()->GetGameInstance()->GetSubsystem<
	ULimenLevelManagerSubsystem>();
	check(LevelManager != nullptr);

	LevelManager->OpenMainMenu();
}
