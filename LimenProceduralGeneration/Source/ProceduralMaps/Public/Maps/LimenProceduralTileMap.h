// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LimenProceduralMap.h"
#include "Templates/SharedPointer.h"
#include "LimenProceduralTileMap.generated.h"


class UTileInfo;
/**
 * 
 */
UCLASS()
class PROCEDURALMAPS_API ULimenProceduralTileMap : public ULimenProceduralMap
{
	GENERATED_BODY()

public:
	const TArray<UTileInfo*>& GetTiles() const;
	
	template<typename T = UTileInfo>
	T* GetTile(const int32 Index);
	
	template<typename T = UTileInfo>
	TArray<T*> GetTiles() const;

	bool IsMapEmpty() const;

	void AddNewTile(UTileInfo* NewTile);
	void ClearTiles();
	void AppendTiles(const TArray<UTileInfo*>& NewTiles);
	void RemoveTile(const int32 Index);
	bool IsIndexValid(const int32 Index) const;

	int32 Count() const;

private:
	UPROPERTY()
	TArray<UTileInfo*> Tiles;
	
};

template <typename T>
T* ULimenProceduralTileMap::GetTile(const int32 Index)
{
	return CastChecked<T>(Tiles[Index]);
}

template <typename T>
TArray<T*> ULimenProceduralTileMap::GetTiles() const
{
	TArray<T*> Out;
	for (UTileInfo* Tile : Tiles)
	{
		Out.Push(CastChecked<T>(Tile));
	}

	return Out;
}
