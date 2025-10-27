// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actors/LimenGameplayManager.h"
#include "MapBuildAlgorithms/LimenMapAlgorithm.h"
#include "LimenProceduralMapBuilder.generated.h"


class ALimenProceduralMapManager;
class UProceduralMapParameters;

UCLASS(Abstract, NotBlueprintable)
class PROCEDURALMAPS_API ALimenProceduralMapBuilder : public ALimenGameplayManager
{
	GENERATED_BODY()
	
public:
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnMapUpdate, const FGuid& Map);
	
	explicit ALimenProceduralMapBuilder(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void LoadMap(const FGuid& MapId);
	void BuildMap(const FGuid& MapId);
	void DestroyMap(const FGuid& MapId);
	void UnloadMap(const FGuid& MapId);

	bool IsMapLoaded(const FGuid& MapId) const;
	bool IsMapBuilt(const FGuid& MapId) const;
	bool DoesMapExist(const FGuid& MapId) const;
	
	const FGuid* GetMapId(const int32 Index) const;
	ALimenProceduralMapManager* GetMapManager(const FGuid& MapId) const;
	int32 GetMapIndex(const FGuid& MapId) const;
	ULimenMapAlgorithm* GetMapAlgorithm(const FGuid& MapId) const;
	const UProceduralMapParameters* GetMapGenerationParameters(const FGuid& MapId) const;
	template<typename T>
	const T* GetMapGenerationParameters(const FGuid& MapId) const
	{
		return Cast<T>(GetMapGenerationParameters(MapId));
	}
	ULimenProceduralMap* GetMap(const FGuid& MapId) const;
	template<typename T>
	T* GetMap(const FGuid& MapId) const
	{
		return Cast<T>(GetMap(MapId));
	}

	TMap<FGuid, TWeakObjectPtr<ULimenProceduralMap>> GetLoadedMaps() const;
	TMap<FGuid, TWeakObjectPtr<ULimenProceduralMap>> GetBuiltMaps() const;
	
	bool IsLastLevel(const FGuid& Test) const;
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
	TMap<FGuid, const UProceduralMapParameters*> MapsParameters;

	virtual void MapBeingLoad(const FGuid& MapId);
	virtual void MapFinishLoad(const FGuid& MapId, ULimenProceduralMap* Map);
	virtual void MapBeginBuild(const FGuid& MapId, ULimenProceduralMap* Map);
	virtual void MapFinishBuild(const FGuid& MapId, ULimenProceduralMap* Map);
	virtual void MapBeginDestroy(const FGuid& MapId, ULimenProceduralMap* Map);
	virtual void MapFinishDestroy(const FGuid& MapId, ULimenProceduralMap* Map);
	virtual void MapBeginUnload(const FGuid& MapId);
	virtual void MapFinishUnload(const FGuid& MapId);
	
private:
	TMap<FGuid, TStrongObjectPtr<ULimenMapAlgorithm>> MapAlgorithms;
	TMap<FGuid, TStrongObjectPtr<ULimenProceduralMap>> LoadedMaps;
	TMap<FGuid, TStrongObjectPtr<ULimenProceduralMap>> BuiltMaps;
	TMap<FGuid, TWeakObjectPtr<ALimenProceduralMapManager>> MapManagers;

	ULimenMapAlgorithm::FAlgorithmFinish AlgorithmFinishDelegate;

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
	
	virtual void StartBuildingMap(const FGuid& MapId, const FMapBuildCallback& FinishCallback);
	virtual void StartDestroyingMap(const FGuid& MapId, const FMapBuildCallback& FinishCallback);
	
};
