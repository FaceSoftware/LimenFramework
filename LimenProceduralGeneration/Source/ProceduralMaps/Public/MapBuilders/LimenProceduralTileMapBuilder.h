// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LimenProceduralMapBuilder.h"
#include "LimenProceduralTileMapBuilder.generated.h"


class UProceduralTileMapParameters;
class ILimenStartTileInterface;
class ILimenEndTileInterface;
class ULimenProceduralTileMap;

UCLASS(Blueprintable)
class PROCEDURALMAPS_API ALimenProceduralTileMapBuilder : public ALimenProceduralMapBuilder
{
	GENERATED_BODY()

public:	
	explicit ALimenProceduralTileMapBuilder(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void BeginPlay() override;

	const TMap<FGuid, TScriptInterface<ILimenStartTileInterface>>& GetStartTiles() const;
	const TMap<FGuid, TScriptInterface<ILimenEndTileInterface>>& GetEndTiles() const;

protected:	
	UPROPERTY(EditDefaultsOnly, Category = "Limen|Tiles Settings", meta=(ClampMin="0")) 
	int64 TileSpawnsPerSecond;
	
	TMap<FGuid, TScriptInterface<ILimenStartTileInterface>>& GetStartTiles();
	TMap<FGuid, TScriptInterface<ILimenEndTileInterface>>& GetEndTiles();
	
private:
	FTimerHandle TileSpawnTimer;
	FTimerHandle TileDespawnTimer;
	int32 CurrentTileSpawnIndex;
	int32 CurrentTileDespawnIndex;

	TMap<FGuid, TScriptInterface<ILimenStartTileInterface>> StartTiles;
	TMap<FGuid, TScriptInterface<ILimenEndTileInterface>> EndTiles;

	void SpawnTilesAsync(const FGuid& MapId, const FMapBuildCallback& FinishCallback);
	void DespawnTilesAsync(const FGuid& MapId, const FMapBuildCallback& FinishCallback);

	void AssignStartAndEndTiles(const FGuid& MapId);
	void DeAssignStartAndEndTiles(const FGuid& MapDataAsset);

	virtual void StartBuildingMap(const FGuid& MapId, const FMapBuildCallback& FinishCallback) override final;
	virtual void StartDestroyingMap(const FGuid& MapId, const FMapBuildCallback& FinishCallback) override final;

};
