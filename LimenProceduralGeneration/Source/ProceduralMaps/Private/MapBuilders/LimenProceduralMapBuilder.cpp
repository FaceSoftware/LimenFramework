// Copyright Face Software. All Rights Reserved.


#include "MapBuilders/LimenProceduralMapBuilder.h"

#include "DataAssets/ProceduralMapParameters.h"
#include "Managers/LimenProceduralMapManager.h"
#include "MapBuildAlgorithms/LimenMapAlgorithm.h"
#include "Maps/LimenProceduralMap.h"


ALimenProceduralMapBuilder::ALimenProceduralMapBuilder(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bTickEvenWhenPaused = false;
	PrimaryActorTick.bCanEverTick = false;
	
	AlgorithmFinishDelegate.BindLambda([this] (bool bSuccess, const FGuid& MapId, ULimenProceduralMap* Map)
	{
		if (!bSuccess)
		{
			return;
		}
		
		MapFinishLoad(MapId, Map);
	});
}

void ALimenProceduralMapBuilder::BeginPlay()
{
	Super::BeginPlay();

#if WITH_EDITOR

	for (const auto& MapDataAsset : Maps)
	{
		ensureMsgf(MapDataAsset.Value != nullptr, TEXT("Map builder contains an invalid data asset"));
	}

#endif
}

void ALimenProceduralMapBuilder::LoadMap(const FGuid& MapId)
{
	check(DoesMapExist(MapId));

	MapBeingLoad(MapId);
	LoadMap_Internal(MapId);
}

void ALimenProceduralMapBuilder::BuildMap(const FGuid& MapId)
{
	check(DoesMapExist(MapId));
	check(IsMapLoaded(MapId));
	check(!IsMapBuilt(MapId));

	
	MapBeginBuild(MapId, GetMap(MapId));
	BuildMap_Internal(MapId);
}

void ALimenProceduralMapBuilder::DestroyMap(const FGuid& MapId)
{
	check(DoesMapExist(MapId));
	check(IsMapBuilt(MapId));

	
	DestroyMap_Internal(MapId);
}

void ALimenProceduralMapBuilder::UnloadMap(const FGuid& MapId)
{
	check(DoesMapExist(MapId));
	check(IsMapLoaded(MapId));

	
	UnloadMap_Internal(MapId);
}

bool ALimenProceduralMapBuilder::IsMapLoaded(const FGuid& MapId) const
{
	return LoadedMaps.Contains(MapId);
}

bool ALimenProceduralMapBuilder::IsMapBuilt(const FGuid& MapId) const
{
	check(Maps.Contains(MapId));
	return BuiltMaps.Contains(MapId);
}

bool ALimenProceduralMapBuilder::DoesMapExist(const FGuid& MapId) const
{
	return Maps.Contains(MapId);
}

const FGuid* ALimenProceduralMapBuilder::GetMapId(const int32 Index) const
{
#if !UE_BUILD_SHIPPING
	{
		TArray<FGuid> Ids;
		Maps.GetKeys(Ids);
		check(Ids.IsValidIndex(Index));
	}
#endif

	uint32 CurrentIndex = 0;
	for (auto& Map : Maps)
	{
		if (CurrentIndex == Index)
		{
			return &Map.Key;
		}
		
		CurrentIndex++;
	}

	return nullptr;
}

ALimenProceduralMapManager* ALimenProceduralMapBuilder::GetMapManager(const FGuid& MapId) const
{
	return MapManagers[MapId];
}

int32 ALimenProceduralMapBuilder::GetMapIndex(const FGuid& MapId) const
{
	check(DoesMapExist(MapId));

	TArray<FGuid> Ids;
	Maps.GenerateKeyArray(Ids);
	
	int32 Index;
	Ids.Find(MapId, Index);
	return Index;
}

ULimenMapAlgorithm* ALimenProceduralMapBuilder::GetMapAlgorithm(const FGuid& MapId) const
{
	return MapAlgorithms[MapId];
}

const UProceduralMapParameters* ALimenProceduralMapBuilder::GetMapGenerationParameters(const FGuid& MapId) const
{
	return Maps[MapId];
}

ULimenProceduralMap* ALimenProceduralMapBuilder::GetMap(const FGuid& MapId) const
{
	ULimenProceduralMap* const* Map = nullptr;
	
	Map = LoadedMaps.Find(MapId);
	if (Map != nullptr)
	{
		return *Map;
	}
	
	Map = BuiltMaps.Find(MapId);
	if (Map != nullptr)
	{
		return *Map;
	}

	return nullptr;
}

const TMap<FGuid, ULimenProceduralMap*>& ALimenProceduralMapBuilder::GetLoadedMaps() const
{
	return LoadedMaps;
}

const TMap<FGuid, ULimenProceduralMap*>& ALimenProceduralMapBuilder::GetBuiltMaps() const
{
	return BuiltMaps;
}

bool ALimenProceduralMapBuilder::IsLastLevel(const FGuid& Test) const
{
	check(!Maps.IsEmpty());
	check(Maps.Contains(Test));

	TArray<FGuid> Ids;
	Maps.GenerateKeyArray(Ids);
	
	return Ids[Ids.Num() - 1] == Test;
}

ALimenProceduralMapBuilder::FOnMapUpdate& ALimenProceduralMapBuilder::GetOnMapBeginBuild()
{
	return OnMapBeginBuild;
}

ALimenProceduralMapBuilder::FOnMapUpdate& ALimenProceduralMapBuilder::GetOnMapFinishBuild()
{
	return OnMapFinishBuild;
}

ALimenProceduralMapBuilder::FOnMapUpdate& ALimenProceduralMapBuilder::GetOnMapBeginLoad()
{
	return OnMapBeginLoad;
}

ALimenProceduralMapBuilder::FOnMapUpdate& ALimenProceduralMapBuilder::GetOnMapFinishLoad()
{
	return OnMapFinishLoad;
}

ALimenProceduralMapBuilder::FOnMapUpdate& ALimenProceduralMapBuilder::GetOnMapBeginUnload()
{
	return OnMapBeginUnload;
}

ALimenProceduralMapBuilder::FOnMapUpdate& ALimenProceduralMapBuilder::GetOnMapFinishUnload()
{
	return OnMapFinishUnload;
}

ALimenProceduralMapBuilder::FOnMapUpdate& ALimenProceduralMapBuilder::GetOnMapBeginDestroy()
{
	return OnMapBeginDestroy;
}

ALimenProceduralMapBuilder::FOnMapUpdate& ALimenProceduralMapBuilder::GetOnMapFinishDestroy()
{
	return OnMapFinishDestroy;
}

void ALimenProceduralMapBuilder::MapBeingLoad(const FGuid& MapId)
{
	GetOnMapBeginLoad().Broadcast(MapId);
}

void ALimenProceduralMapBuilder::MapFinishLoad(const FGuid& MapId, ULimenProceduralMap* Map)
{
	LoadedMaps.Add(MapId, Map);
	
	GetOnMapFinishLoad().Broadcast(MapId);
}

void ALimenProceduralMapBuilder::MapBeginBuild(const FGuid& MapId, ULimenProceduralMap* Map)
{
	GetOnMapBeginBuild().Broadcast(MapId);
}

void ALimenProceduralMapBuilder::MapFinishBuild(const FGuid& MapId, ULimenProceduralMap* Map)
{
	UClass* ManagerClass = Maps[MapId]->GetManagerClass();
	
	ALimenProceduralMapManager* Manager = GetWorld()->SpawnActor<ALimenProceduralMapManager>(ManagerClass);
	check(Manager != nullptr);
	
	MapManagers.Add(MapId, Manager);
	Manager->MapBuilt(Map);
	BuiltMaps.Add(MapId, Map);
	
	GetOnMapFinishBuild().Broadcast(MapId);
}

void ALimenProceduralMapBuilder::MapBeginDestroy(const FGuid& MapId, ULimenProceduralMap* Map)
{
	GetOnMapBeginDestroy().Broadcast(MapId);
}

void ALimenProceduralMapBuilder::MapFinishDestroy(const FGuid& MapId, ULimenProceduralMap* Map)
{
	verify(MapManagers[MapId]->Destroy());
	MapManagers.Remove(MapId);
		
	if (!LoadedMaps.Contains(MapId))
	{
		verify(BuiltMaps[MapId]->ConditionalBeginDestroy());
	}
	
	BuiltMaps.Remove(MapId);

	GetOnMapFinishDestroy().Broadcast(MapId);
}

void ALimenProceduralMapBuilder::MapBeginUnload(const FGuid& MapId)
{
	GetOnMapBeginUnload().Broadcast(MapId);
}

void ALimenProceduralMapBuilder::MapFinishUnload(const FGuid& MapId)
{
	check(LoadedMaps.Contains(MapId));

	if (!BuiltMaps.Contains(MapId))
	{
		verify(LoadedMaps[MapId]->ConditionalBeginDestroy());
	}
	LoadedMaps.Remove(MapId);
	
	verify(MapAlgorithms[MapId]->ConditionalBeginDestroy());
	MapAlgorithms.Remove(MapId);

	GetOnMapFinishUnload().Broadcast(MapId);
}

void ALimenProceduralMapBuilder::LoadMap_Internal(const FGuid& MapId)
{	
	const UProceduralMapParameters* Params = Maps[MapId];
	check(Params->GetGenerationAlgorithm() != nullptr);
	
	ULimenMapAlgorithm* MapAlgorithm = NewObject<ULimenMapAlgorithm>(this, Params->GetGenerationAlgorithm());
	MapAlgorithm->CreateMap(MapId, Params, AlgorithmFinishDelegate);
	
	MapAlgorithms.Add(MapId, MapAlgorithm);
}

void ALimenProceduralMapBuilder::BuildMap_Internal(const FGuid& MapId)
{
	StartBuildingMap(MapId, [this, MapId] (const bool bSuccess,
										   ULimenProceduralMap* Map)
	{
		if (!bSuccess)
		{
			return;
		}
		
		MapFinishBuild(MapId, Map);
	});
}

void ALimenProceduralMapBuilder::DestroyMap_Internal(const FGuid& MapId)
{
	check(BuiltMaps.Contains(MapId));

	StartDestroyingMap(MapId, [this, MapId] (const bool bSuccess,
										   ULimenProceduralMap* Map)
	{
		if (!bSuccess)
		{
			return;
		}

		MapFinishDestroy(MapId, Map);
	});
}

void ALimenProceduralMapBuilder::UnloadMap_Internal(const FGuid& MapId)
{
	MapFinishUnload(MapId);
}

void ALimenProceduralMapBuilder::StartBuildingMap(const FGuid& MapId,
	const FMapBuildCallback& FinishCallback)
{
	PURE_VIRTUAL(ALimenProceduralMapBuilder::StartBuildingMap);
}

void ALimenProceduralMapBuilder::StartDestroyingMap(const FGuid& MapId,
	const FMapBuildCallback& FinishCallback)
{
	PURE_VIRTUAL(ALimenProceduralMapBuilder::StartDestroyingMap);
}
