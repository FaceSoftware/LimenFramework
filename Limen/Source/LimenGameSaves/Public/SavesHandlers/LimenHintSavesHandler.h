// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SaveGames/LimenSaveData.h"
#include "SavesHandlers/LimenSavesHandler.h"
#include "LimenHintSavesHandler.generated.h"

struct FActorSaveData;
/**
 * 
 */
UCLASS()
class LIMENGAMESAVES_API ULimenHintSavesHandler : public ULimenSavesHandler
{
	GENERATED_BODY()

public:
	virtual bool LoadDataTo(UWorld* World) override;
	virtual bool SaveDataFrom(UWorld* World) override;

private:
	UPROPERTY(SaveGame)
	FActorSaveData HintsManager;
};
