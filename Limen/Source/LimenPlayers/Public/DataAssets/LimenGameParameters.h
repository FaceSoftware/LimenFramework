// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LimenGameParameters.generated.h"

/**
 * 
 */
UCLASS()
class LIMENPLAYERS_API ULimenGameParameters : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UWorld> LevelAsset = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString LevelOptions;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	uint8 Difficulty = 0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 NumberOfPlayers = 1;
};
