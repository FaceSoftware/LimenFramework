// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SaveGames/LimenSaveData.h"
#include "LimenGameSaveData.generated.h"

/**
 * 
 */
UCLASS()
class LIMENGAMESAVES_API ULimenGameSaveData : public ULimenSaveData
{
	GENERATED_BODY()

public:
	UPROPERTY(SaveGame)
	TArray<FObjectSaveData> StoredSaveHandlers;
};
