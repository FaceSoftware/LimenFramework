// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameModes/LimenGameModeBase.h"
#include "LimenMenuGameMode.generated.h"


class ULimenGameParameters;
class ULimenSaveData;
class UGameParams;

UCLASS(Blueprintable)
class LIMENGAMEMODES_API ALimenMenuGameMode : public ALimenGameModeBase
{
	GENERATED_BODY()

public:
	ALimenMenuGameMode();

	virtual void PostLogin(APlayerController* NewPlayer) override;

	void StartNewGame(APlayerController* Player, ULimenGameParameters* GameParams);
	void StartLoadingGame(APlayerController* Player, ULimenSaveData* SaveGame);

protected:
	

private:
	
};