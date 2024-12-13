// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SaveGames/LimenSaveData.h"
#include "LimenModularSaveData.generated.h"

/**
 * 
 */
UCLASS()
class LIMENSAVESYSTEM_API ULimenModularSaveData : public ULimenSaveData
{
	GENERATED_BODY()

public:
	UPROPERTY(SaveGame)
	TArray<FObjectSaveData> StoredSaveHandlers;
};
