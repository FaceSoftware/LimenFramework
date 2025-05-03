// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actors/LimenGameplayManager.h"
#include "LimenProceduralMapManager.generated.h"


class ULimenProceduralMap;
class ALimenProceduralMapBuilder;
class UProceduralMapParameters;

UCLASS(Abstract)
class PROCEDURALMAPS_API ALimenProceduralMapManager : public ALimenGameplayManager
{
	GENERATED_BODY()

public:
	DECLARE_MULTICAST_DELEGATE(FMapComplete)

	/**
	 * @brief Called from the map builder class to notify the manager that the map has been built.
	 * Do not call this directly!
	 * @param Map The built map;
	 */
	virtual void MapBuilt(ULimenProceduralMap* Map);

	UFUNCTION(BlueprintCallable)
	void NotifyMapComplete();

	FMapComplete& GetMapCompleteDelegate();
	ALimenProceduralMapBuilder* GetTileMapBuilder();


protected:

private:
	FMapComplete OnMapComplete;
	
};
