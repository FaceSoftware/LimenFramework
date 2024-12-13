// Copyright Face Software. All Rights Reserved.


#include "MapBuildAlgorithms/LimenTileMapAlgorithm.h"

#include "DataAssets/ProceduralTileMapParameters.h"
#include "Maps/LimenProceduralTileMap.h"
#include "Misc/LowLevelTestAdapter.h"
#include "Subsystems/LimenGlobalRandomStreamSubsystem.h"
#include "Tiles/LimenMapTile.h"


TSubclassOf<ULimenProceduralMap> ULimenTileMapAlgorithm::GetGeneratedMapClass() const
{
	return ULimenProceduralTileMap::StaticClass();
}

bool ULimenTileMapAlgorithm::GetTilesByClass(const TSubclassOf<ALimenMapTile>& TileClass, TArray<UTileInfo*>& OutTiles) const
{
	for (const auto& Tile : GetGeneratedTileMap()->GetTiles())
	{
		if (Tile->TileClass == TileClass)
		{
			OutTiles.Push(Tile);
		}
	}

	return !OutTiles.IsEmpty();
}

bool ULimenTileMapAlgorithm::CreateTileInfoFromLocation(const FVector& Location, const EMapTileDirection& Direction,
														UTileInfo*& OutTileInfo) const
{
	CHECK(OutTileInfo != nullptr);
	
	FVector BuildLocation = Location;

	switch (Direction)
	{
	case EMapTileDirection::Left:
		BuildLocation -= FVector(0.f, GetGeneratedTileMapParameters()->GetTileSize(), 0.f);
		break;

	case EMapTileDirection::Right:
		BuildLocation += FVector(0.f, GetGeneratedTileMapParameters()->GetTileSize(), 0.f);
		break;

	case EMapTileDirection::Forward:
		BuildLocation += FVector(GetGeneratedTileMapParameters()->GetTileSize(), 0.f, 0.f);
		break;

	case EMapTileDirection::Backwards:
		BuildLocation -= FVector(GetGeneratedTileMapParameters()->GetTileSize(), 0.f, 0.f);
		break;

	default:
		checkNoEntry()
		break;
	}
	
	OutTileInfo->Location = BuildLocation;
	return true;
}

bool ULimenTileMapAlgorithm::TileExistsInLocation(const FVector& Location) const
{
	for (const auto& TileInfo : GetGeneratedTileMap()->GetTiles())
	{
		if (TileInfo->Location == Location)
		{
			return true;
		}
	}

	return false;
}

bool ULimenTileMapAlgorithm::GetTileInLocation(const FVector& Location, UTileInfo*& OutTileInfo) const
{
	for (UTileInfo* TileInfo : GetGeneratedTileMap()->GetTiles())
	{
		if (TileInfo->Location == Location)
		{
			OutTileInfo = TileInfo;
			return true;
		}
	}

	return false;
}

void ULimenTileMapAlgorithm::RemoveTileInLocation(const FVector& Location) const
{
	int32 Index = 0;
	for (auto& Tile : GetGeneratedTileMap()->GetTiles())
	{
		if (Tile->Location == Location)
		{
			GetGeneratedTileMap()->RemoveTile(Index);
			return;
		}

		Index++;
	}
}

bool ULimenTileMapAlgorithm::TileExistsInDirection(const FVector& Location, const EMapTileDirection Direction) const
{
	FVector LocationToCheck = Location;

	switch (Direction)
	{
	case EMapTileDirection::Left:
		LocationToCheck -= FVector(0.f, GetGeneratedTileMapParameters()->GetTileSize(), 0.f);
		break;

	case EMapTileDirection::Right:
		LocationToCheck += FVector(0.f, GetGeneratedTileMapParameters()->GetTileSize(), 0.f);
		break;

	case EMapTileDirection::Forward:
		LocationToCheck += FVector(GetGeneratedTileMapParameters()->GetTileSize(), 0.f, 0.f);
		break;

	case EMapTileDirection::Backwards:
		LocationToCheck -= FVector(GetGeneratedTileMapParameters()->GetTileSize(), 0.f, 0.f);
		break;

	default:
		return false;
	}

	return TileExistsInLocation(LocationToCheck);
}

bool ULimenTileMapAlgorithm::GetTileInDirection(const FVector& Location, const EMapTileDirection Direction,
												UTileInfo*& OutTileInfo) const
{
	FVector LocationToCheck = Location;

	switch (Direction)
	{
	case EMapTileDirection::Left:
		LocationToCheck -= FVector(0.f, GetGeneratedTileMapParameters()->GetTileSize(), 0.f);
		break;

	case EMapTileDirection::Right:
		LocationToCheck += FVector(0.f, GetGeneratedTileMapParameters()->GetTileSize(), 0.f);
		break;

	case EMapTileDirection::Forward:
		LocationToCheck += FVector(GetGeneratedTileMapParameters()->GetTileSize(), 0.f, 0.f);
		break;

	case EMapTileDirection::Backwards:
		LocationToCheck -= FVector(GetGeneratedTileMapParameters()->GetTileSize(), 0.f, 0.f);
		break;

	default:
		return false;
	}

	return GetTileInLocation(LocationToCheck, OutTileInfo);
}

bool ULimenTileMapAlgorithm::GetClosestTileToLocation(const FVector& Location, UTileInfo*& OutTileInfo) const
{
	if (GetGeneratedTileMap()->IsMapEmpty())
	{
		return false;
	}

	float MinDistance = std::numeric_limits<float>::max();
	bool bFound = false;

	for (UTileInfo* Tile : GetGeneratedTileMap()->GetTiles())
	{
		const float CurrentDistance = FVector::DistSquared(Location, Tile->Location);

		if (CurrentDistance < MinDistance)
		{
			MinDistance = CurrentDistance;
			OutTileInfo = Tile;
			bFound = true;
		}
	}

	return bFound;
}

EMapTileDirection ULimenTileMapAlgorithm::GetOppositeDirection(const EMapTileDirection& CurrentDirection)
{
	switch (CurrentDirection)
	{
	case EMapTileDirection::Left:
		return EMapTileDirection::Right;
		
	case EMapTileDirection::Right:
		return EMapTileDirection::Left;
		
	case EMapTileDirection::Forward:
		return EMapTileDirection::Backwards;
		
	case EMapTileDirection::Backwards:
		return EMapTileDirection::Forward;
		
	default:
		checkNoEntry();
		break;
	}

	return EMapTileDirection::None;
}

void ULimenTileMapAlgorithm::RemoveTileInDirection(const FVector& Location, const EMapTileDirection Direction) const
{
	FVector LocationToCheck = Location;

	switch (Direction)
	{
	case EMapTileDirection::Left:
		LocationToCheck -= FVector(0.f, GetGeneratedTileMapParameters()->GetTileSize(), 0.f);
		break;

	case EMapTileDirection::Right:
		LocationToCheck += FVector(0.f, GetGeneratedTileMapParameters()->GetTileSize(), 0.f);
		break;

	case EMapTileDirection::Forward:
		LocationToCheck += FVector(GetGeneratedTileMapParameters()->GetTileSize(), 0.f, 0.f);
		break;

	case EMapTileDirection::Backwards:
		LocationToCheck -= FVector(GetGeneratedTileMapParameters()->GetTileSize(), 0.f, 0.f);
		break;

	default:
		checkNoEntry()
		break;
	}

	RemoveTileInLocation(LocationToCheck);
}

bool ULimenTileMapAlgorithm::GetRandomTileInfoIndexOfType(const ETileType TileType, uint64& OutIndex) const
{	
	TArray<uint64> CompatibleTiles;
	for (int i = 0; i < GetGeneratedTileMap()->GetTiles().Num(); i++)
	{
		if (TileType == GetGeneratedTileMap()->GetTile(i)->Type)
		{
			CompatibleTiles.Add(i);
		}
	}

	if (CompatibleTiles.IsEmpty())
	{
		return false;
	}

	TArray<int> RandomIndex = ULimenGlobalRandomStreamSubsystem::Get()->GenerateRandomUniqueNumbers(CompatibleTiles.Num() - 1, 0, 1);
	OutIndex = CompatibleTiles[RandomIndex[0]];
	return true;
}

void ULimenTileMapAlgorithm::SeparateTiles(const TArray<TSubclassOf<ALimenMapTile>>& InTiles,
                                           TArray<TSubclassOf<ALimenMapTile>>* OutDeadEnd, TArray<TSubclassOf<ALimenMapTile>>* OutLinearCorridor,
                                           TArray<TSubclassOf<ALimenMapTile>>* OutCurvedCorridor, TArray<TSubclassOf<ALimenMapTile>>* OutTJunction,
                                           TArray<TSubclassOf<ALimenMapTile>>* OutCrossIntersection)
{
	for (const auto& Class : InTiles)
	{
		if (Class->GetDefaultObject<ALimenMapTile>()->GetTileType() == ETileType::CrossIntersection&& OutCrossIntersection != nullptr)
		{
			OutCrossIntersection->Push(Class);
		}
		else if (Class->GetDefaultObject<ALimenMapTile>()->GetTileType() == ETileType::TJunction && OutTJunction != nullptr)
		{
			OutTJunction->Push(Class);
		}
		else if (Class->GetDefaultObject<ALimenMapTile>()->GetTileType() == ETileType::LinearCorridor && OutLinearCorridor != nullptr)
		{
			OutLinearCorridor->Push(Class);
		}
		else if (Class->GetDefaultObject<ALimenMapTile>()->GetTileType() == ETileType::CurvedCorridor && OutCurvedCorridor != nullptr)
		{
			OutCurvedCorridor->Push(Class);
		}
		else if (Class->GetDefaultObject<ALimenMapTile>()->GetTileType() == ETileType::DeadEnd && OutDeadEnd != nullptr)
		{
			OutDeadEnd->Push(Class);
		}
		else
		{
			checkNoEntry()
		}
	}
}

void ULimenTileMapAlgorithm::AssignTileType() const
{
}


void ULimenTileMapAlgorithm::AssignTileRotation() const
{
}

void ULimenTileMapAlgorithm::ReplaceTilesWithRequired() const
{
	if (GetGeneratedTileMapParameters()->GetRequiredTiles().IsEmpty())
	{
		return;
	}

	const TArray<TSubclassOf<ALimenMapTile>>& RequiredTiles = GetGeneratedTileMapParameters()->GetRequiredTiles();

	TArray<uint64> UsedIndexes;
	for (int i = 0; i < RequiredTiles.Num(); ++i)
	{
		bool bIsValidIndex = true;
		uint64 TileIndex = 0;
		verify(GetRandomTileInfoIndexOfType(RequiredTiles[i]->GetDefaultObject<ALimenMapTile>()->GetTileType(), TileIndex))

		for (const uint64& UsedIndex : UsedIndexes)
		{
			if (UsedIndex == TileIndex)
			{
				bIsValidIndex = false;
			}
		}
		
		if (!bIsValidIndex)
		{
			i--;
			continue;
		}
		
		GetGeneratedTileMap()->GetTile(TileIndex)->TileClass = RequiredTiles[i];
		UsedIndexes.Push(TileIndex);
	}
}

bool ULimenTileMapAlgorithm::RequiredTilesHaveValidPlace() const
{
	const TArray<TSubclassOf<ALimenMapTile>>& RequiredTiles = GetGeneratedTileMapParameters()->GetRequiredTiles();
	
	if (RequiredTiles.IsEmpty())
	{
		return true;
	}
	
	TArray<TSubclassOf<ALimenMapTile>> RequiredDeadEndTiles;
	TArray<TSubclassOf<ALimenMapTile>> RequiredLinearCorridorTiles;
	TArray<TSubclassOf<ALimenMapTile>> RequiredCurvedCorridorTiles;
	TArray<TSubclassOf<ALimenMapTile>> RequiredTJunctionTiles;
	TArray<TSubclassOf<ALimenMapTile>> RequiredCrossIntersectionTiles;
	SeparateTiles(RequiredTiles, &RequiredDeadEndTiles, &RequiredLinearCorridorTiles, &RequiredCurvedCorridorTiles, &RequiredTJunctionTiles, &RequiredCrossIntersectionTiles);

	uint32 Dead = 0, Linear = 0, Curved = 0, T = 0, Cross = 0;
	const uint32 RequiredDead = RequiredDeadEndTiles.Num();
	const uint32 RequiredLinear = RequiredLinearCorridorTiles.Num();
	const uint32 RequiredCurved = RequiredCurvedCorridorTiles.Num();
	const uint32 RequiredT = RequiredTJunctionTiles.Num();
	const uint32 RequiredCross = RequiredCrossIntersectionTiles.Num();
	
	for (const auto& CurrentTile : GetGeneratedTileMap()->GetTiles())
	{
		const ETileType CurrentTileType = CurrentTile->Type;
		check(CurrentTileType != ETileType::Undefined);
		
		if (CurrentTileType == ETileType::CrossIntersection)
		{
			Cross++;
		}
		else if (CurrentTileType == ETileType::TJunction)
		{
			T++;
		}
		else if (CurrentTileType == ETileType::LinearCorridor)
		{
			Linear++;
		}
		else if (CurrentTileType == ETileType::CurvedCorridor)
		{
			Curved++;
		}
		else if (CurrentTileType == ETileType::DeadEnd)
		{
			Dead++;
		}
		else
		{
			checkNoEntry()
		}
	}

	return Dead >= RequiredDead && Linear >= RequiredLinear && Curved >= RequiredCurved && T >= RequiredT && Cross >= RequiredCross;
}

void ULimenTileMapAlgorithm::AssignTileClass() const
{
}

void ULimenTileMapAlgorithm::GenerateMap(const UProceduralMapParameters* MapParameters,
                                         ULimenProceduralMap*& OutGeneratedMap)
{
	GeneratedTileMap = CastChecked<ULimenProceduralTileMap>(OutGeneratedMap);
	GeneratedTileMapParameters = CastChecked<UProceduralTileMapParameters>(MapParameters);
}

ULimenProceduralTileMap* ULimenTileMapAlgorithm::GetGeneratedTileMap() const
{
	return GeneratedTileMap.Get();
}

const UProceduralTileMapParameters* ULimenTileMapAlgorithm::GetGeneratedTileMapParameters() const
{
	return GeneratedTileMapParameters.Get();
}
