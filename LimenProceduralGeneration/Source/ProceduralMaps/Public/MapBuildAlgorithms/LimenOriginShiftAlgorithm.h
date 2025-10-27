// Copyright FaceSoftware. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LimenTileMapAlgorithm.h"
#include "Tiles/LimenMapTile.h"
#include "LimenOriginShiftAlgorithm.generated.h"


class ULimenGlobalRandomStreamSubsystem;

UCLASS(Blueprintable, BlueprintType)
class PROCEDURALMAPS_API UOriginShiftTile : public UTileInfo
{
	GENERATED_BODY()

public:
	EMapTileDirection Direction;
};

/**
 * 
 */
UCLASS(Blueprintable)
class PROCEDURALMAPS_API ULimenOriginShiftAlgorithm : public ULimenTileMapAlgorithm
{
	GENERATED_BODY()
	
public:	
	ULimenOriginShiftAlgorithm();	
	virtual TSubclassOf<ULimenProceduralMap> GetGeneratedMapClass() const override;

protected:
	UPROPERTY(EditAnywhere)
	int32 NumberOfIterations;
	
	virtual void GenerateMap(const UProceduralMapParameters* MapParameters, ULimenProceduralMap*& OutGeneratedMap) override;

private:
	struct FTileConnections
	{
		bool bFront = false, bBack = false, bLeft = false, bRight = false;
		int32 Count = 0;
	};

	TWeakObjectPtr<ULimenGlobalRandomStreamSubsystem> GlobalRandStream;

	int32 SideTileLength;

	UOriginShiftTile* GetTile(const FIntVector2& Coordinates) const;
	FIntVector2 GetTileCoordinates(const int32 Index) const;
	bool AreTilesConnected(const UOriginShiftTile* L, const UOriginShiftTile* R) const;
	
	void GenerateSimpleMap();
	
	UOriginShiftTile* GetOrigin(int32* OutIndex = nullptr) const;
	FTileConnections GetTileConnections(const UOriginShiftTile* Tile) const;
	
	bool ShiftOrigin(const EMapTileDirection InDirection) const;
	void ShiftOriginInRandomDirection() const;

	virtual void AssignTileType() const override;
	virtual void AssignTileRotation() const override;
	virtual void AssignTileClass() const override;
};
