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
	ULimenProceduralMap* GetMap(const FGuid& MapId) const;

	const TMap<FGuid, ULimenProceduralMap*>& GetLoadedMaps() const;
	const TMap<FGuid, ULimenProceduralMap*>& GetBuiltMaps() const;
	
	bool IsLastLevel(const FGuid& Test) const;

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
	TMap<FGuid, const UProceduralMapParameters*> Maps;

	virtual void MapBeingLoad(const FGuid& MapId);
	virtual void MapFinishLoad(const FGuid& MapId, ULimenProceduralMap* Map);
	virtual void MapBeginBuild(const FGuid& MapId, ULimenProceduralMap* Map);
	virtual void MapFinishBuild(const FGuid& MapId, ULimenProceduralMap* Map);
	virtual void MapBeginDestroy(const FGuid& MapId, ULimenProceduralMap* Map);
	virtual void MapFinishDestroy(const FGuid& MapId, ULimenProceduralMap* Map);
	virtual void MapBeginUnload(const FGuid& MapId);
	virtual void MapFinishUnload(const FGuid& MapId);
	
private:
	UPROPERTY()
	TMap<FGuid, ULimenMapAlgorithm*> MapAlgorithms;
	UPROPERTY()
	TMap<FGuid, ULimenProceduralMap*> LoadedMaps;
	UPROPERTY()
	TMap<FGuid, ULimenProceduralMap*> BuiltMaps;
	UPROPERTY()
	TMap<FGuid, ALimenProceduralMapManager*> MapManagers;

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

	void LoadMap_Internal(const FGuid& MapId);
	void BuildMap_Internal(const FGuid& MapId);
	void DestroyMap_Internal(const FGuid& MapId);
	void UnloadMap_Internal(const FGuid& MapId);
	
	virtual void StartBuildingMap(const FGuid& MapId, const FMapBuildCallback& FinishCallback);
	virtual void StartDestroyingMap(const FGuid& MapId, const FMapBuildCallback& FinishCallback);
	
};
