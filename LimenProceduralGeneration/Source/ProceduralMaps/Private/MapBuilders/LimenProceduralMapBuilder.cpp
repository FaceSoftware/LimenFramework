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
	bReplicates = false;
	MapsBuilt = 0;
	
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
	
	for (int32 CollectionIdx = 0; CollectionIdx < MapCollections.Num(); ++CollectionIdx)
	{
		for (const auto& MapData : MapCollections[CollectionIdx].GroupMaps)
		{
			MapsParameters.Add(MapData.Key, MapData.Value.LoadSynchronous());
		}
	}

#if WITH_EDITOR

	for (const auto& MapDataAsset : MapsParameters)
	{
		ensureMsgf(MapDataAsset.Value != nullptr, TEXT("Map builder contains an invalid data asset"));
	}

#endif
}

void ALimenProceduralMapBuilder::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	for (const auto& Element : MapAlgorithms)
	{
		Element.Value->ConditionalBeginDestroy();
	}
	MapAlgorithms.Empty();
	for (const auto& Element : LoadedMaps)
	{
		Element.Value->ConditionalBeginDestroy();
	}
	LoadedMaps.Empty();
	for (const auto& Element : BuiltMaps)
	{
		Element.Value->ConditionalBeginDestroy();
	}
	BuiltMaps.Empty();

	Super::EndPlay(EndPlayReason);
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
	check(MapsParameters.Contains(MapId));
	return BuiltMaps.Contains(MapId);
}

bool ALimenProceduralMapBuilder::DoesMapExist(const FGuid& MapId) const
{
	return MapsParameters.Contains(MapId);
}

const FGuid* ALimenProceduralMapBuilder::GetMapFromCollection(const FName& CollectionName,
	const FRandomStreamRef& InRandomStream) const
{
	const FProceduralMapGroup* TargetGroup = MapCollections.FindByPredicate([&CollectionName](const FProceduralMapGroup& Group)
	{
		return Group.GroupName == CollectionName;
	});
	
	if (!TargetGroup) { return nullptr; }

	const int32 RandomIndex = InRandomStream->RandRange(0, TargetGroup->GroupMaps.Num() - 1);
	int32 CurrentIndex = 0;
	for (auto It = TargetGroup->GroupMaps.CreateConstIterator(); It; ++It)
	{
		if (CurrentIndex == RandomIndex)
		{
			return &It.Key();
		}
		CurrentIndex++;
	}

	return nullptr;
}

int32 ALimenProceduralMapBuilder::GetCollectionMapCount(const FName& CollectionName) const
{
	const FProceduralMapGroup* TargetGroup = MapCollections.FindByPredicate([&CollectionName](const FProceduralMapGroup& Group)
	{
		return Group.GroupName == CollectionName;
	});
	
	return TargetGroup->GroupMaps.Num();
}

ALimenProceduralMapManager* ALimenProceduralMapBuilder::GetMapManager(const FGuid& MapId) const
{
	return MapManagers[MapId].Get();
}

ULimenMapAlgorithm* ALimenProceduralMapBuilder::GetMapAlgorithm(const FGuid& MapId) const
{
	return MapAlgorithms[MapId].Get();
}

const UProceduralMapParameters* ALimenProceduralMapBuilder::GetMapGenerationParameters(const FGuid& MapId) const
{
	return MapsParameters[MapId].LoadSynchronous();
}

ULimenProceduralMap* ALimenProceduralMapBuilder::GetMap(const FGuid& MapId) const
{	
	TStrongObjectPtr<ULimenProceduralMap> const* Map = LoadedMaps.Find(MapId);
	if (Map != nullptr) { return Map->Get(); }
	
	Map = BuiltMaps.Find(MapId);
	if (Map != nullptr) { return Map->Get(); }

	return nullptr;
}

TMap<FGuid, TWeakObjectPtr<ULimenProceduralMap>> ALimenProceduralMapBuilder::GetLoadedMaps() const
{
	TMap<FGuid, TWeakObjectPtr<ULimenProceduralMap>> Out;
	for (const auto& LoadedMap : LoadedMaps)
	{
		Out.Add(LoadedMap.Key, TWeakObjectPtr(LoadedMap.Value.Get()));
	}
	return Out;
}

TMap<FGuid, TWeakObjectPtr<ULimenProceduralMap>> ALimenProceduralMapBuilder::GetBuiltMaps() const
{
	TMap<FGuid, TWeakObjectPtr<ULimenProceduralMap>> Out;
	for (const auto& BuiltMap : BuiltMaps)
	{
		Out.Add(BuiltMap.Key, TWeakObjectPtr(BuiltMap.Value.Get()));
	}
	return Out;
}

int32 ALimenProceduralMapBuilder::GetMapsBuilt() const
{
	return MapsBuilt;
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
	LoadedMaps.Add(MapId, TStrongObjectPtr(Map));
	
	GetOnMapFinishLoad().Broadcast(MapId);
}

void ALimenProceduralMapBuilder::MapBeginBuild(const FGuid& MapId, ULimenProceduralMap* Map)
{	
	UClass* ManagerClass = MapsParameters[MapId].LoadSynchronous()->GetManagerClass();
	ALimenProceduralMapManager* Manager = GetWorld()->SpawnActor<ALimenProceduralMapManager>(ManagerClass);
	check(Manager != nullptr);
	MapManagers.Add(MapId, TWeakObjectPtr(Manager));
	
	GetOnMapBeginBuild().Broadcast(MapId);
}

void ALimenProceduralMapBuilder::MapFinishBuild(const FGuid& MapId, ULimenProceduralMap* Map)
{
	MapsBuilt++;
	BuiltMaps.Add(MapId, TStrongObjectPtr(Map));
	
	MapManagers[MapId]->MapBuilt(Map);
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
	const UProceduralMapParameters* Params = MapsParameters[MapId].LoadSynchronous();
	check(Params->GetGenerationAlgorithm() != nullptr);
	
	ULimenMapAlgorithm* MapAlgorithm = NewObject<ULimenMapAlgorithm>(this, Params->GetGenerationAlgorithm());
	MapAlgorithm->CreateMap(MapId, Params, AlgorithmFinishDelegate);
	
	MapAlgorithms.Add(MapId, TStrongObjectPtr(MapAlgorithm));
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
		GetOnMapFinishBuild().Broadcast(MapId);
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
