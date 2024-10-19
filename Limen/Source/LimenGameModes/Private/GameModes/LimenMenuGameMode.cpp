// Copyright Face Software. All Rights Reserved.


#include "GameModes/LimenMenuGameMode.h"

#include "DataAssets/LimenGameParameters.h"
#include "PlayerControllers/LimenMenuPlayerController.h"
#include "Subsystems/LimenLevelManagerSubsystem.h"


ALimenMenuGameMode::ALimenMenuGameMode()
{
	bUseSeamlessTravel = false;
}

void ALimenMenuGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	auto* LimenPlayer = Cast<ALimenMenuPlayerController>(NewPlayer);
	if (LimenPlayer == nullptr)
	{
		return;
	}

	LimenPlayer->OnNewGameRequested.AddUObject(this, &ThisClass::StartNewGame);
	LimenPlayer->OnLoadGameRequested.AddUObject(this, &ThisClass::StartLoadingGame);
}

void ALimenMenuGameMode::StartNewGame(APlayerController* Player, ULimenGameParameters* GameParams)
{
	ULimenLevelManagerSubsystem* LevelManager = GetWorld()->GetGameInstance()->GetSubsystem<ULimenLevelManagerSubsystem>();
	check(LevelManager != nullptr);
	
	LevelManager->OpenLocalLevel(GameParams->LevelAsset, true, GameParams->LevelOptions);
}

void ALimenMenuGameMode::StartLoadingGame(APlayerController* Player, ULimenSaveData* SaveGame)
{
}
