// Copyright Face Software. All Rights Reserved.


#include "DataAssets/ProceduralTileMapParameters.h"


bool UProceduralTileMapParameters::ValidateParameters() const
{
	return NumberOfTiles > RequiredTiles.Num();
}

TSubclassOf<ALimenProceduralMapManager> UProceduralTileMapParameters::GetManagerClass() const
{
	return MapManagerClass;
}

TSubclassOf<ULimenMapAlgorithm> UProceduralTileMapParameters::GetGenerationAlgorithm() const
{
	return GenerationAlgorithm;
}

float UProceduralTileMapParameters::GetTileSize() const
{
	return TileSize;
}

uint64 UProceduralTileMapParameters::GetNumberOfTiles() const
{
	return NumberOfTiles;
}

FVector UProceduralTileMapParameters::GetStartPosition() const
{
	return StartPosition;
}

const TArray<TSubclassOf<ALimenMapTile>>& UProceduralTileMapParameters::GetRequiredTiles() const
{
	return RequiredTiles;
}

const TArray<TSubclassOf<ALimenMapTile>>& UProceduralTileMapParameters::GetDeadEndTiles() const
{
	return DeadEnd;
}

const TArray<TSubclassOf<ALimenMapTile>>& UProceduralTileMapParameters::GetLinearCorridorTiles() const
{
	return LinearCorridor;
}

const TArray<TSubclassOf<ALimenMapTile>>& UProceduralTileMapParameters::GetCurvedCorridorTiles() const
{
	return CurvedCorridor;
}

const TArray<TSubclassOf<ALimenMapTile>>& UProceduralTileMapParameters::GetTJunctionTiles() const
{
	return TJunction;
}

const TArray<TSubclassOf<ALimenMapTile>>& UProceduralTileMapParameters::GetCrossIntersectionTiles() const
{
	return CrossIntersection;
}
