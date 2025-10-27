// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LimenMapAlgorithm.h"
#include "LimenTileMapAlgorithm.generated.h"


class UTileInfo;
class UProceduralTileMapParameters;
class ULimenProceduralTileMap;
enum class ETileType : uint8;
struct FTileInfo;
class ALimenMapTile;

UENUM(BlueprintType)
enum class EMapTileDirection : uint8
{
	None		= 0,
	Left		= 1,
	Right       = 2,
	Forward     = 3,
	Backwards   = 4,
};

/**
 * 
 */
UCLASS(BlueprintType)
class PROCEDURALMAPS_API ULimenTileMapAlgorithm : public ULimenMapAlgorithm
{
	GENERATED_BODY()

public:
	virtual TSubclassOf<ULimenProceduralMap> GetGeneratedMapClass() const override;
	
	bool GetTilesByClass(const TSubclassOf<ALimenMapTile>& TileClass, TArray<UTileInfo*>& OutTiles) const;
	bool TileExistsInLocation(const FVector& Location) const;
	bool TileExistsInDirection(const FVector& Location, const EMapTileDirection Direction) const;
	bool GetTileInLocation(const FVector& Location, UTileInfo*& OutTileInfo) const;
	bool GetTileInDirection(const FVector& Location, const EMapTileDirection Direction, UTileInfo*& OutTileInfo) const;
	bool GetClosestTileToLocation(const FVector& Location, UTileInfo*& OutTileInfo) const;
	static EMapTileDirection GetOppositeDirection(const EMapTileDirection& CurrentDirection);

protected:
	bool CreateTileInfoFromLocation(const FVector& Location, const EMapTileDirection& Direction, UTileInfo*& OutTileInfo) const;
	void RemoveTileInDirection(const FVector& Location, const EMapTileDirection Direction) const;
	void RemoveTileInLocation(const FVector& Location) const;
	bool GetRandomTileInfoIndexOfType(const ETileType TileType, uint64& OutIndex) const;

	static void SeparateTiles(const TArray<TSubclassOf<ALimenMapTile>>& InTiles,
		TArray<TSubclassOf<ALimenMapTile>>* OutDeadEnd, TArray<TSubclassOf<ALimenMapTile>>* OutLinearCorridor,
		TArray<TSubclassOf<ALimenMapTile>>* OutCurvedCorridor, TArray<TSubclassOf<ALimenMapTile>>* OutTJunction,
		TArray<TSubclassOf<ALimenMapTile>>* OutCrossIntersection);

	virtual void AssignTileType() const;
	virtual void AssignTileRotation() const;
	virtual void ReplaceTilesWithRequired() const;
	virtual bool RequiredTilesHaveValidPlace() const;
	virtual void AssignTileClass() const;

	virtual void GenerateMap(const UProceduralMapParameters* MapParameters, ULimenProceduralMap*& OutGeneratedMap) override;

	ULimenProceduralTileMap* GetGeneratedTileMap() const;
	const UProceduralTileMapParameters* GetGeneratedTileMapParameters() const;
	
private:
	TWeakObjectPtr<ULimenProceduralTileMap> GeneratedTileMap;
	TWeakObjectPtr<const UProceduralTileMapParameters> GeneratedTileMapParameters;
};
