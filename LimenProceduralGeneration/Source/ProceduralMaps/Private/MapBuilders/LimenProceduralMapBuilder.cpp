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
	
	AlgorithmFinishDelegate.BindUObject(this, &ThisClass::AlgorithmFinish);
}

void ALimenProceduralMapBuilder::BeginPlay()
{
	Super::BeginPlay();

#if WITH_EDITOR

	for (const FProceduralMapGroup& Collection : MapCollections)
	{
		for (auto& Map : Collection.GroupMaps)
		{
			ensureMsgf(!Map.IsNull(), TEXT("Map builder contains an invalid data asset"));
		}
	}

#endif
}

void ALimenProceduralMapBuilder::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	MapInstances.Empty();
	
	AlgorithmFinishDelegate.Unbind();

	Super::EndPlay(EndPlayReason);
}

FGuid ALimenProceduralMapBuilder::LoadMap(const FName GroupName, const int32 Index)
{
	check(DoesMapExist(GroupName, Index));

	const FGuid MapId = FGuid::NewGuid();

	MapInstances.Add(MapId, FMapInstance(GroupName, Index));
	
	MapBeingLoad(MapId);
	LoadMap_Internal(MapId);
	return MapId;
}

void ALimenProceduralMapBuilder::BuildMap(const FGuid& MapId)
{
	check(IsMapLoaded(MapId))
	check(!IsMapBuilt(MapId))
	
	MapBeginBuild(MapId, GetMapData(MapId));
	BuildMap_Internal(MapId);
}

void ALimenProceduralMapBuilder::DestroyMap(const FGuid& MapId)
{
	check(IsMapBuilt(MapId))

	DestroyMap_Internal(MapId);
}

void ALimenProceduralMapBuilder::UnloadMap(const FGuid& MapId)
{
	check(IsMapLoaded(MapId));
	
	UnloadMap_Internal(MapId);
}

bool ALimenProceduralMapBuilder::IsMapLoaded(const FGuid& MapId) const
{
	const FMapInstance* Instance = MapInstances.Find(MapId);
	if (!Instance) { return false; }
	
	return Instance->bIsLoaded;
}

bool ALimenProceduralMapBuilder::IsMapBuilt(const FGuid& MapId) const
{
	const FMapInstance* Instance = MapInstances.Find(MapId);
	if (!Instance) { return false; }
	
	return Instance->bIsBuilt;
}

bool ALimenProceduralMapBuilder::DoesMapExist(const FName GroupName, const int32 Index) const
{
	return MapCollections.FindByPredicate([&GroupName, &Index] (const FProceduralMapGroup& Test)
	{
		return Test.GroupName == GroupName && Test.GroupMaps.IsValidIndex(Index);
	}) != nullptr;
}

int32 ALimenProceduralMapBuilder::GetCollectionMapCount(const FName& CollectionName) const
{
	const FProceduralMapGroup* TargetGroup = MapCollections.FindByPredicate([&CollectionName](const FProceduralMapGroup& Group)
	{
		return Group.GroupName == CollectionName;
	});
	
	return TargetGroup->GroupMaps.Num();
}

TArray<TWeakObjectPtr<ULimenProceduralMap>> ALimenProceduralMapBuilder::GetLoadedMapsData() const
{
	TArray<TWeakObjectPtr<ULimenProceduralMap>> Out;
	for (const auto& Map : MapInstances)
	{
		if (Map.Value.bIsLoaded)
		{
			Out.Add(TWeakObjectPtr(Map.Value.MapData.Get()));
		}
	}
	return Out;
}

TArray<TWeakObjectPtr<ULimenProceduralMap>> ALimenProceduralMapBuilder::GetBuiltMapsData() const
{
	TArray<TWeakObjectPtr<ULimenProceduralMap>> Out;
	for (const auto& Map : MapInstances)
	{
		if (Map.Value.bIsBuilt)
		{
			Out.Add(TWeakObjectPtr(Map.Value.MapData.Get()));
		}
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
	check(!IsMapLoaded(MapId));
	GetOnMapBeginLoad().Broadcast(MapId);
}

void ALimenProceduralMapBuilder::MapFinishLoad(const FGuid& MapId, ULimenProceduralMap* Map)
{
	check(!IsMapLoaded(MapId));
	FMapInstance& Instance = MapInstances.FindChecked(MapId);
	Instance.MapData = TStrongObjectPtr(Map);
	Instance.bIsLoaded = true;
}

void ALimenProceduralMapBuilder::MapBeginBuild(const FGuid& MapId, ULimenProceduralMap* Map)
{
	check(!IsMapBuilt(MapId))
	
	FMapInstance& Instance = MapInstances.FindChecked(MapId);
	
	UClass* ManagerClass = Instance.MapParameters->GetManagerClass();
	ALimenProceduralMapManager* Manager = GetWorld()->SpawnActor<ALimenProceduralMapManager>(ManagerClass);
	
	Instance.MapManager = TWeakObjectPtr(Manager);
	
	GetOnMapBeginBuild().Broadcast(MapId);
}

void ALimenProceduralMapBuilder::MapFinishBuild(const FGuid& MapId, ULimenProceduralMap* Map)
{
	check(!IsMapBuilt(MapId))

	MapsBuilt++;

	FMapInstance& Instance = MapInstances.FindChecked(MapId);
	Instance.bIsBuilt = true;
}

void ALimenProceduralMapBuilder::MapBeginDestroy(const FGuid& MapId, ULimenProceduralMap* Map)
{
	check(IsMapBuilt(MapId))
	GetOnMapBeginDestroy().Broadcast(MapId);
}

void ALimenProceduralMapBuilder::MapFinishDestroy(const FGuid& MapId, ULimenProceduralMap* Map)
{
	check(IsMapBuilt(MapId))
	
	FMapInstance& Instance = MapInstances.FindChecked(MapId);
	Instance.bIsBuilt = false;

	check(Instance.MapManager.IsValid())
	Instance.MapManager->Destroy();
	Instance.MapManager.Reset();
}

void ALimenProceduralMapBuilder::MapBeginUnload(const FGuid& MapId)
{
	check(IsMapLoaded(MapId));
	
	GetOnMapBeginUnload().Broadcast(MapId);
}

void ALimenProceduralMapBuilder::MapFinishUnload(const FGuid& MapId)
{
	check(IsMapLoaded(MapId));

	MapInstances.Remove(MapId);
}

void ALimenProceduralMapBuilder::LoadMap_Internal(const FGuid& MapId)
{
	FMapInstance& Instance = MapInstances.FindChecked(MapId);
	const FName MapGroupName = Instance.MapGroup;
	const int32 MapIndex = Instance.MapIndex;
	
	FProceduralMapGroup* Group = MapCollections.FindByKey(MapGroupName);
	check(Group)
	
	UProceduralMapParameters* Parameters = Group->GroupMaps[MapIndex].LoadSynchronous();
	check(Parameters)
	check(Parameters->ValidateParameters());

	const TSubclassOf<ULimenMapAlgorithm> AlgorithmClass = Parameters->GetGenerationAlgorithm();
	check(AlgorithmClass);
	
	ULimenMapAlgorithm* Algorithm = NewObject<ULimenMapAlgorithm>(this, AlgorithmClass);
	check(Algorithm)

	Instance.MapParameters = TStrongObjectPtr(Parameters);
	Instance.MapAlgorithm = TStrongObjectPtr(Algorithm);
	
	Algorithm->CreateMap(MapId, Parameters, AlgorithmFinishDelegate);
}

void ALimenProceduralMapBuilder::BuildMap_Internal(const FGuid& MapId)
{
	TWeakObjectPtr WeakThis = this;
	StartBuildingMap(MapId, [WeakThis, MapId] (const bool bSuccess,
										   ULimenProceduralMap* Map)
	{
		if (!WeakThis.IsValid() || !bSuccess) { return; }
		
		WeakThis->MapFinishBuild(MapId, Map);
		WeakThis->GetOnMapFinishBuild().Broadcast(MapId);
	});
}

void ALimenProceduralMapBuilder::DestroyMap_Internal(const FGuid& MapId)
{
	check(IsMapBuilt(MapId));

	TWeakObjectPtr WeakThis = this;
	StartDestroyingMap(MapId, [WeakThis, MapId] (const bool bSuccess,
										   ULimenProceduralMap* Map)
	{
		if (!WeakThis.IsValid() || !bSuccess) { return; }

		WeakThis->MapFinishDestroy(MapId, Map);
		WeakThis->GetOnMapFinishDestroy().Broadcast(MapId);
	});
}

void ALimenProceduralMapBuilder::UnloadMap_Internal(const FGuid& MapId)
{
	MapFinishUnload(MapId);
	GetOnMapFinishUnload().Broadcast(MapId);
}

void ALimenProceduralMapBuilder::AlgorithmFinish(const bool bSuccess, const FGuid& MapId, ULimenProceduralMap* Map)
{
	if (!bSuccess)
	{
		MapInstances.Remove(MapId);
		return;
	}
		
	MapFinishLoad(MapId, Map);
	GetOnMapFinishLoad().Broadcast(MapId);
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
