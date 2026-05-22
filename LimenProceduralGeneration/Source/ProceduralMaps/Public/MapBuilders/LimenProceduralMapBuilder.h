// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actors/LimenGameplayManager.h"
#include "MapBuildAlgorithms/LimenMapAlgorithm.h"
#include "Managers/LimenProceduralMapManager.h"
#include "Maps/LimenProceduralMap.h"
#include "DataAssets/ProceduralMapParameters.h"
#include "LimenProceduralMapBuilder.generated.h"


USTRUCT(BlueprintType)
struct FProceduralMapGroup
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Maps")
	FName GroupName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Maps")
	TArray<TSoftObjectPtr<UProceduralMapParameters>> GroupMaps;
	
	bool operator==(const FName& InGroupName) const
	{
		return GroupName == InGroupName;
	}
};


UCLASS(Abstract, NotBlueprintable)
class PROCEDURALMAPS_API ALimenProceduralMapBuilder : public ALimenGameplayManager
{
	GENERATED_BODY()
	
public:
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnMapUpdate, const FGuid& Map);
	
	explicit ALimenProceduralMapBuilder(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void NotifyActorRegistry(const FGameplayTag& Id, AActor* Actor) override;

	FGuid LoadMap(const FName GroupName, const int32 Index);
	void BuildMap(const FGuid& MapId);
	void DestroyMap(const FGuid& MapId);
	void UnloadMap(const FGuid& MapId);

	bool IsMapLoaded(const FGuid& MapId) const;
	bool IsMapBuilt(const FGuid& MapId) const;
	bool DoesMapExist(const FName GroupName, const int32 Index) const;

	int32 GetCollectionMapCount(const FName& CollectionName) const;

	template<typename T = ALimenProceduralMapManager>
	FORCEINLINE T* GetMapManager(const FGuid& MapId) const
	{
		static_assert(TIsDerivedFrom<T, ALimenProceduralMapManager>::Value);
		
		const FMapInstance* Instance = MapInstances.Find(MapId);
		if (!Instance) { return nullptr; }
		return Cast<T>(Instance->MapManager.Get());
	}
	
	template<typename T = ULimenMapAlgorithm>
	FORCEINLINE T* GetMapAlgorithm(const FGuid& MapId) const
	{
		static_assert(TIsDerivedFrom<T, ULimenMapAlgorithm>::Value);
		
		const FMapInstance* Instance = MapInstances.Find(MapId);
		if (!Instance) { return nullptr; }
		return Cast<T>(Instance->MapAlgorithm.Get());
	}

	template<typename T = UProceduralMapParameters>
	FORCEINLINE T* GetMapGenerationParameters(const FGuid& MapId) const
	{
		static_assert(TIsDerivedFrom<T, UProceduralMapParameters>::Value);
		
		const FMapInstance* Instance = MapInstances.Find(MapId);
		if (!Instance) { return nullptr; }
		return Cast<T>(Instance->MapParameters.Get());
	}

	template<typename T = ULimenProceduralMap>
	FORCEINLINE T* GetMapData(const FGuid& MapId) const
	{
		static_assert(TIsDerivedFrom<T, ULimenProceduralMap>::Value);
		
		const FMapInstance* Instance = MapInstances.Find(MapId);
		if (!Instance) { return nullptr; }
		return Cast<T>(Instance->MapData.Get());
	}

	TArray<TWeakObjectPtr<ULimenProceduralMap>> GetLoadedMapsData() const;
	TArray<TWeakObjectPtr<ULimenProceduralMap>> GetBuiltMapsData() const;

	int32 GetMapsBuilt() const;

	FOnMapUpdate& GetOnMapBeginBuild();
	FOnMapUpdate& GetOnMapFinishBuild();
	FOnMapUpdate& GetOnMapBeginLoad();
	FOnMapUpdate& GetOnMapFinishLoad();
	FOnMapUpdate& GetOnMapBeginUnload();
	FOnMapUpdate& GetOnMapFinishUnload();
	FOnMapUpdate& GetOnMapBeginDestroy();
	FOnMapUpdate& GetOnMapFinishDestroy();

protected:
	typedef TFunction<void(bool, ULimenProceduralMap*)> FMapBuildCallback;
	
	UPROPERTY(EditDefaultsOnly, Category="Maps")
	TArray<FProceduralMapGroup> MapCollections;
	
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void MapBeingLoad(const FGuid& MapId);
	virtual void MapFinishLoad(const FGuid& MapId, ULimenProceduralMap* Map);
	virtual void MapBeginBuild(const FGuid& MapId, ULimenProceduralMap* Map);
	virtual void MapFinishBuild(const FGuid& MapId, ULimenProceduralMap* Map);
	virtual void MapBeginDestroy(const FGuid& MapId, ULimenProceduralMap* Map);
	virtual void MapFinishDestroy(const FGuid& MapId, ULimenProceduralMap* Map);
	virtual void MapBeginUnload(const FGuid& MapId);
	virtual void MapFinishUnload(const FGuid& MapId);
	
private:
	struct FMapInstance
	{
		FMapInstance() = default;
		explicit FMapInstance(const FName& InMapGroup, const int32 InMapIndex)
		{
			MapGroup = InMapGroup;
			MapIndex = InMapIndex;
			bIsLoaded = false;
			bIsBuilt = false;
		}
		
		FName MapGroup;
		int32 MapIndex;

		TStrongObjectPtr<UProceduralMapParameters> MapParameters;
		TStrongObjectPtr<ULimenMapAlgorithm> MapAlgorithm;
		TStrongObjectPtr<ULimenProceduralMap> MapData;
		TStrongObjectPtr<ALimenProceduralMapManager> MapManager;
		
		bool bIsLoaded;
		bool bIsBuilt;
	};
	
	TMap<FGuid, FMapInstance> MapInstances;

	ULimenMapAlgorithm::FAlgorithmFinish AlgorithmFinishDelegate;
	FDelegateHandle AlgorithmFinishDelegateHandle;

	bool bIsWorkingFlag;

	FOnMapUpdate OnMapBeginBuild;
	FOnMapUpdate OnMapFinishBuild;
	FOnMapUpdate OnMapBeginLoad;
	FOnMapUpdate OnMapFinishLoad;
	FOnMapUpdate OnMapBeginUnload;
	FOnMapUpdate OnMapFinishUnload;
	FOnMapUpdate OnMapBeginDestroy;
	FOnMapUpdate OnMapFinishDestroy;

	int32 MapsBuilt;

	void LoadMap_Internal(const FGuid& MapId);
	void BuildMap_Internal(const FGuid& MapId);
	void DestroyMap_Internal(const FGuid& MapId);
	void UnloadMap_Internal(const FGuid& MapId);
	
	void AlgorithmFinish(bool bSuccess, const FGuid& MapId, ULimenProceduralMap* Map);
	
	virtual void StartBuildingMap(const FGuid& MapId, const FMapBuildCallback& FinishCallback);
	virtual void StartDestroyingMap(const FGuid& MapId, const FMapBuildCallback& FinishCallback);
};
