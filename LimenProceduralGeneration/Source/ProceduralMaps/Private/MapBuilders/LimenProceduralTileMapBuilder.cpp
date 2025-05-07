// Copyright Face Software. All Rights Reserved.


#include "MapBuilders/LimenProceduralTileMapBuilder.h"

#include "TimerManager.h"
#include "DataAssets/ProceduralMapParameters.h"
#include "Interfaces/LimenEndTileInterface.h"
#include "Interfaces/LimenStartTileInterface.h"
#include "Managers/LimenProceduralMapManager.h"
#include "Maps/LimenProceduralTileMap.h"
#include "Tiles/LimenMapTile.h"


ALimenProceduralTileMapBuilder::ALimenProceduralTileMapBuilder(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bRunOnAnyThread = false;
	
	TileSpawnsPerSecond = 20;
	CurrentTileSpawnIndex = 0;
	CurrentTileDespawnIndex = 0;
}

void ALimenProceduralTileMapBuilder::BeginPlay()
{
	Super::BeginPlay();

	check(TileSpawnsPerSecond > 0);
}

const TMap<FGuid, TScriptInterface<ILimenStartTileInterface>>& ALimenProceduralTileMapBuilder::GetStartTiles() const
{
	return StartTiles;
}

const TMap<FGuid, TScriptInterface<ILimenEndTileInterface>>& ALimenProceduralTileMapBuilder::GetEndTiles() const
{
	return EndTiles;
}

TMap<FGuid, TScriptInterface<ILimenStartTileInterface>>& ALimenProceduralTileMapBuilder::GetStartTiles()
{
	return StartTiles;
}

TMap<FGuid, TScriptInterface<ILimenEndTileInterface>>& ALimenProceduralTileMapBuilder::GetEndTiles()
{
	return EndTiles;
}

void ALimenProceduralTileMapBuilder::MapFinishBuild(const FGuid& MapId, ULimenProceduralMap* Map)
{
	Super::MapFinishBuild(MapId, Map);

	ALimenProceduralMapManager* MapManager = GetMapManager(MapId);
	for (const UTileInfo* Tile : GetMap<ULimenProceduralTileMap>(MapId)->GetTiles())
	{
		check(MapManager != nullptr);
		Tile->TileActor->SetMapManager(MapManager);
	}
}

void ALimenProceduralTileMapBuilder::SpawnTilesAsync(const FGuid& MapId, const FMapBuildCallback& FinishCallback)
{
	check(CurrentTileSpawnIndex == 0);
	
	FActorSpawnParameters Params;
	Params.Owner = this;
	Params.bNoFail = true;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	const float SpawnRate = 1.f / TileSpawnsPerSecond;
	check(!FMath::IsNearlyZero(SpawnRate) || FMath::IsNegativeOrNegativeZero(SpawnRate));
	
	ULimenProceduralTileMap* Map = CastChecked<ULimenProceduralTileMap>(GetLoadedMaps()[MapId]);

	GetWorld()->GetTimerManager().SetTimer(TileSpawnTimer, [this, Params, Map, FinishCallback]
	{
		UTileInfo* TileInfo = Map->GetTile(CurrentTileSpawnIndex);
		
		UClass* Class = TileInfo->TileClass;
		const FVector& Location = TileInfo->Location;
		const FRotator& Rotation = TileInfo->Rotation;
		
		TileInfo->TileActor = GetWorld()->SpawnActor<ALimenMapTile>(Class, Location, Rotation, Params);

		if (CurrentTileSpawnIndex >= Map->Count() - 1)
		{
			CurrentTileSpawnIndex = 0;
			FinishCallback.CheckCallable();
			FinishCallback(true, Map);
			
			GetWorld()->GetTimerManager().ClearTimer(TileSpawnTimer);
			return;
		}
		
		CurrentTileSpawnIndex++;
	}
	, SpawnRate, true);
}

void ALimenProceduralTileMapBuilder::DespawnTilesAsync(const FGuid& MapId, const FMapBuildCallback& FinishCallback)
{
	check(CurrentTileDespawnIndex == 0);
	
	const float DespawnRate = 1.f / TileSpawnsPerSecond;
	ULimenProceduralTileMap* Map = CastChecked<ULimenProceduralTileMap>(GetBuiltMaps()[MapId]);
	GetWorld()->GetTimerManager().SetTimer(TileDespawnTimer, [this, Map, FinishCallback]
	{
		UTileInfo* TileInfo = Map->GetTile(CurrentTileDespawnIndex);
		
		check(TileInfo->TileActor != nullptr);
		TileInfo->TileActor->Destroy();

		if (CurrentTileDespawnIndex >= Map->Count() - 1)
		{
			CurrentTileDespawnIndex = 0;
			FinishCallback.CheckCallable();
			FinishCallback(true, Map);

			GetWorld()->GetTimerManager().ClearTimer(TileDespawnTimer);
			return;
		}
		
		CurrentTileDespawnIndex++;
	}
	, DespawnRate, true);
}

void ALimenProceduralTileMapBuilder::AssignStartAndEndTiles(const FGuid& MapId)
{
	ULimenProceduralMap* Map = GetLoadedMaps()[MapId];
	const ULimenProceduralTileMap* TileMap = CastChecked<ULimenProceduralTileMap>(Map);

	uint8 AssignmentCount = 0;
	for (const UTileInfo* Tile : TileMap->GetTiles())
	{
		check(Tile != nullptr);
		if (Tile->TileActor->Implements<ULimenStartTileInterface>())
		{
			ALimenMapTile* TileActor = Tile->TileActor.Get();
			StartTiles.Add(MapId, TScriptInterface<ILimenStartTileInterface>(TileActor));
			AssignmentCount++;
		}
		if (Tile->TileActor->Implements<ULimenEndTileInterface>())
		{
			ALimenMapTile* TileActor = Tile->TileActor.Get();
			EndTiles.Add(MapId, TScriptInterface<ILimenEndTileInterface>(TileActor));
			AssignmentCount++;
		}

		if (AssignmentCount == 2)
		{
			break;
		}
	}
}

void ALimenProceduralTileMapBuilder::DeAssignStartAndEndTiles(const FGuid& MapId)
{
	StartTiles.Remove(MapId);
	EndTiles.Remove(MapId);
}

void ALimenProceduralTileMapBuilder::StartBuildingMap(const FGuid& MapId, const FMapBuildCallback& FinishCallback)
{
	ULimenProceduralTileMap* TileMapPtr = CastChecked<ULimenProceduralTileMap>(GetLoadedMaps()[MapId]);
	
	SpawnTilesAsync(MapId, [this, MapId, TileMapPtr, FinishCallback] (const bool bSuccess, ULimenProceduralMap* BuiltMap)
	{		
		AssignStartAndEndTiles(MapId);

		FinishCallback.CheckCallable();
		FinishCallback(bSuccess, BuiltMap);
	});
}

void ALimenProceduralTileMapBuilder::StartDestroyingMap(const FGuid& MapId, const FMapBuildCallback& FinishCallback)
{	
	DespawnTilesAsync(MapId, [this, MapId, FinishCallback] (bool, ULimenProceduralMap* DestroyedMap)
	{
		DeAssignStartAndEndTiles(MapId);

		FinishCallback.CheckCallable();
		FinishCallback(true, DestroyedMap);
	});
}
