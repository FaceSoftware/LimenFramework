// Copyright Face Software. All Rights Reserved.


#include "Maps/LimenProceduralTileMap.h"

#include "Tiles/LimenMapTile.h"


const TArray<UTileInfo*>& ULimenProceduralTileMap::GetTiles() const
{
	return Tiles;
}

bool ULimenProceduralTileMap::IsMapEmpty() const
{
	return Tiles.IsEmpty();
}

void ULimenProceduralTileMap::AddNewTile(UTileInfo* NewTile)
{
	Tiles.Push(NewTile);
}

void ULimenProceduralTileMap::ClearTiles()
{
	for (auto& Tile : Tiles)
	{
		verify(Tile->ConditionalBeginDestroy());
	}
	Tiles.Empty(128);
}

void ULimenProceduralTileMap::AppendTiles(const TArray<UTileInfo*>& NewTiles)
{
	Tiles.Append(NewTiles);
}

void ULimenProceduralTileMap::RemoveTile(const int32 Index)
{
	Tiles.RemoveAt(Index, EAllowShrinking::Yes);
}

bool ULimenProceduralTileMap::IsIndexValid(const int32 Index) const
{
	return Tiles.IsValidIndex(Index);
}

int32 ULimenProceduralTileMap::Count() const
{
	return Tiles.Num();
}
