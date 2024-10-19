// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actors/LimenGameplayManager.h"
#include "LimenProceduralMapManager.generated.h"

class ALimenProceduralMapBuilder;
class UProceduralMapDataAsset;

UCLASS(Abstract)
class PROCEDURALMAPS_API ALimenProceduralMapManager : public ALimenGameplayManager
{
	GENERATED_BODY()

	DECLARE_MULTICAST_DELEGATE(FMapCompleteDelegate);

public:
	FMapCompleteDelegate OnMapComplete;
	
	virtual void MapBuilt(UProceduralMapDataAsset* MapData);

protected:
	/**
	 * @brief Notifies the map builder that the map is complete and starts loading the next map.
	 * @note Const version.
	 */
	UFUNCTION()
	void NotifyMapComplete() const;
	/**
	 * @brief Notifies the map builder that the map is complete and starts loading the next map.
	 * @note Non-const version.
	 */
	void NotifyMapComplete();

	template<typename T = ALimenProceduralMapBuilder>
	T* GetGameplayManager() const
	{
		static_assert(std::is_base_of_v<ALimenProceduralMapBuilder, T>());
		
		if (MapBuilder == nullptr)
		{
			MapBuilder = ALimenGameplayManager::GetGameplayManager<T>(this);
		}
		
		return static_cast<T>(MapBuilder);
	}

private:
	UPROPERTY()
	TWeakObjectPtr<ALimenProceduralMapBuilder> MapBuilder;
	
};
