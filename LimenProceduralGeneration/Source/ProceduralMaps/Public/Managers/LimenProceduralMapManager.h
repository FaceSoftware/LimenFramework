// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actors/LimenGameplayManager.h"
#include "LimenProceduralMapManager.generated.h"


class ULimenProceduralMap;
class ALimenProceduralMapBuilder;
class UProceduralMapParameters;

UCLASS(Blueprintable)
class PROCEDURALMAPS_API ALimenProceduralMapManager : public AInfo
{
	GENERATED_BODY()

public:
	DECLARE_MULTICAST_DELEGATE(FMapComplete)
	DECLARE_MULTICAST_DELEGATE(FMapEnter)

	/**
	 * @brief Called from the map builder class to notify the manager that the map has been built.
	 * Do not call this directly!
	 * @param Map The built map;
	 */
	virtual void MapBuilt(ULimenProceduralMap* Map);

	virtual void NotifyMapComplete();
	virtual void NotifyMapEntered();

	FMapComplete& GetMapCompleteDelegate();
	FMapEnter& GetMapEnteredDelegate();
	template<typename T = ALimenProceduralMapBuilder>
	T* GetMapBuilder()
	{
		static_assert(TIsDerivedFrom<T, ALimenProceduralMapBuilder>::Value);
		auto* Builder = ALimenGameplayManager::GetGameplayManager(
			this, T::StaticClass());
		return Cast<T>(Builder);
	}


protected:

private:
	FMapComplete OnMapComplete;
	FMapEnter OnMapEntered;
	
};
