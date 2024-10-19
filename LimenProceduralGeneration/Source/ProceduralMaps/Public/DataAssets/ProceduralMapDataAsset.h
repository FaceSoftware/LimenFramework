// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ProceduralMapDataAsset.generated.h"

class ALimenProceduralMapBuilder;
class ALimenProceduralMapManager;
/**
 * 
 */
UCLASS()
class PROCEDURALMAPS_API UProceduralMapDataAsset : public UDataAsset
{
	GENERATED_BODY()

	friend ALimenProceduralMapBuilder;

public:
	TSubclassOf<ALimenProceduralMapManager> GetMapManagerClass() const
	{
		return MapManagerClass;
	}

private:
	UPROPERTY(EditAnywhere, Category = "Limen|Generation Parameters")
	TSubclassOf<ALimenProceduralMapManager> MapManagerClass;
};
