// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameMode/LimenGameModeBase.h"
#include "LimenMenuGameMode.generated.h"


class ULimenSaveData;

UCLASS(Blueprintable)
class LIMENGAMEMODES_API ALimenMenuGameMode : public ALimenGameModeBase
{
	GENERATED_BODY()

public:
	ALimenMenuGameMode();

	virtual void PostLogin(APlayerController* NewPlayer) override;

	void StartNewGame(APlayerController* Player);
	void StartLoadingGame(APlayerController* Player, const ULimenSaveData* SaveGame);

protected:
	

private:
	
};