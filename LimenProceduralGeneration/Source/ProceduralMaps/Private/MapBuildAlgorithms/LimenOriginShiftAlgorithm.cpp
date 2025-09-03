// Copyright FaceSoftware. All Rights Reserved.


#include "MapBuildAlgorithms/LimenOriginShiftAlgorithm.h"

#include "BlueprintLibraries/LimenCoreStatics.h"
#include "BlueprintLibraries/LimenMath.h"
#include "DataAssets/ProceduralTileMapParameters.h"
#include "Maps/LimenProceduralTileMap.h"
#include "Tiles/LimenMapTile.h"
#include "Subsystems/LimenGlobalRandomStreamSubsystem.h"


ULimenOriginShiftAlgorithm::ULimenOriginShiftAlgorithm()
{
	NumberOfIterations = 1024;
	SideTileLength = 0;
}

TSubclassOf<ULimenProceduralMap> ULimenOriginShiftAlgorithm::GetGeneratedMapClass() const
{
	return Super::GetGeneratedMapClass();
}

void ULimenOriginShiftAlgorithm::GenerateMap(const UProceduralMapParameters* MapParameters,
										  ULimenProceduralMap*& OutGeneratedMap)
{
	Super::GenerateMap(MapParameters, OutGeneratedMap);

	GlobalRandStream = ULimenGlobalRandomStreamSubsystem::Get();
	check(GlobalRandStream != nullptr);
	
	GenerateSimpleMap();
	
	for (int i = 0; i < NumberOfIterations; ++i)
	{
		ShiftOriginInRandomDirection();
	}

	// In case the iterations did not generate a valid map,
	// make sure to actually create a valid map before proceeding
	AssignTileType();
	while (!RequiredTilesHaveValidPlace())
	{
		ShiftOriginInRandomDirection();
		AssignTileType();
	}

	AssignTileRotation();
	AssignTileClass();
	ReplaceTilesWithRequired();
}

UOriginShiftTile* ULimenOriginShiftAlgorithm::GetTile(const FIntVector2& Coordinates) const
{
	check(Coordinates.X >= 0 && Coordinates.X <= SideTileLength - 1);
	check(Coordinates.Y >= 0 && Coordinates.Y <= SideTileLength - 1);

	const int32 TileIndex = Coordinates.X * SideTileLength + Coordinates.Y;
	check(GetGeneratedTileMap()->GetTiles().IsValidIndex(TileIndex))
	
	return GetGeneratedTileMap()->GetTile<UOriginShiftTile>(TileIndex);
}

FIntVector2 ULimenOriginShiftAlgorithm::GetTileCoordinates(const int32 Index) const
{
	check(GetGeneratedTileMap()->IsIndexValid(Index));

	FIntVector2 OutCoordinates;
	OutCoordinates.Y = Index % SideTileLength;
	OutCoordinates.X = Index / SideTileLength;

	check(OutCoordinates.X >= 0 && OutCoordinates.X <= SideTileLength - 1);
	check(OutCoordinates.Y >= 0 && OutCoordinates.Y <= SideTileLength - 1);
	
	return OutCoordinates;
}

bool ULimenOriginShiftAlgorithm::AreTilesConnected(const UOriginShiftTile* L, const UOriginShiftTile* R) const
{
	check(L != nullptr);
	check(R != nullptr);
	UTileInfo* TempTileInfo = nullptr;

	if (L->Direction != EMapTileDirection::None)
	{
		verify(GetTileInDirection(L->Location, L->Direction, TempTileInfo));
		if (TempTileInfo == R)
		{
			return true;
		}
	}

	if (R->Direction != EMapTileDirection::None)
	{
		verify(GetTileInDirection(R->Location, R->Direction, TempTileInfo));
		if (TempTileInfo == L)
		{
			return true;
		}
	}

	return false;
}

void ULimenOriginShiftAlgorithm::GenerateSimpleMap()
{
	const uint64 TileCount = GetGeneratedTileMapParameters()->GetNumberOfTiles();
	const float FloatSideTileCount = FMath::Sqrt(static_cast<double>(TileCount));

	checkf(ULimenMath::IsInteger(FloatSideTileCount), TEXT("This algorithm only supports square maps."));
	SideTileLength = FloatSideTileCount;

	const FVector InitialLocation = GetGeneratedTileMapParameters()->GetStartPosition();
	FVector CurrentBuildLocation = InitialLocation;

	int32 RowNumber = 0;
	for (int32 i = 0; i < TileCount; ++i)
	{
		UOriginShiftTile* NewTile = NewObject<UOriginShiftTile>();
		UTileInfo* TempTile = NewTile;
		verify(CreateTileInfoFromLocation(CurrentBuildLocation, EMapTileDirection::Right, TempTile));

		if ((i + 1) % SideTileLength != 0)
		{
			NewTile->Direction = EMapTileDirection::Right;
 			CurrentBuildLocation.Y += GetGeneratedTileMapParameters()->GetTileSize();
		}
		else // Is last tile from row
		{
			if (i < TileCount - 1) // Is last tile
			{
				NewTile->Direction = EMapTileDirection::Forward;
			}
			else
			{
				NewTile->Direction = EMapTileDirection::None;
			}
			
			CurrentBuildLocation = InitialLocation;
			CurrentBuildLocation.X += GetGeneratedTileMapParameters()->GetTileSize() * ++RowNumber;
		}

		GetGeneratedTileMap()->AddNewTile(NewTile);
	}
}

UOriginShiftTile* ULimenOriginShiftAlgorithm::GetOrigin(int32* OutIndex) const
{
	int32 Index = 0;
	for (UOriginShiftTile*& Tile : GetGeneratedTileMap()->GetTiles<UOriginShiftTile>())
	{
		if (Tile->Direction == EMapTileDirection::None)
		{
			*OutIndex = Index;
			return Tile;
		}
		
		Index++;
	}

	checkNoEntry()
	return nullptr;
}

ULimenOriginShiftAlgorithm::FTileConnections ULimenOriginShiftAlgorithm::GetTileConnections(const UOriginShiftTile* Tile) const
{
	FTileConnections Connections;

	UTileInfo* TestTile;
	bool bTileExistsInDirection;
	{
		bTileExistsInDirection = GetTileInDirection(Tile->Location, EMapTileDirection::Backwards, TestTile);
		if (bTileExistsInDirection)
		{
			if (const UOriginShiftTile* BackTile = CastChecked<UOriginShiftTile>(TestTile);
				AreTilesConnected(Tile, BackTile))
			{
				Connections.Count++;
				Connections.bBack = true;
			}
		}
	}

	{
		bTileExistsInDirection = GetTileInDirection(Tile->Location, EMapTileDirection::Forward, TestTile);
		if (bTileExistsInDirection)
		{
			if (const UOriginShiftTile* ForwardTile = CastChecked<UOriginShiftTile>(TestTile);
				AreTilesConnected(Tile, ForwardTile))
			{
				Connections.Count++;
				Connections.bFront = true;
			}
		}
	}

	{
		bTileExistsInDirection = GetTileInDirection(Tile->Location, EMapTileDirection::Left, TestTile);
		if (bTileExistsInDirection)
		{
			if (const UOriginShiftTile* LeftTile = CastChecked<UOriginShiftTile>(TestTile);
				AreTilesConnected(Tile, LeftTile))
			{
				Connections.Count++;
				Connections.bLeft = true;
			}
		}
	}

	{
		bTileExistsInDirection = GetTileInDirection(Tile->Location, EMapTileDirection::Right, TestTile);
		if (bTileExistsInDirection)
		{
			if (const UOriginShiftTile* RightTile = CastChecked<UOriginShiftTile>(TestTile);
				AreTilesConnected(Tile, RightTile))
			{
				Connections.Count++;
				Connections.bRight = true;
			}
		}
	}

	check(Connections.Count == Connections.bFront + Connections.bBack + Connections.bLeft + Connections.bRight);
	return Connections;
}

bool ULimenOriginShiftAlgorithm::ShiftOrigin(const EMapTileDirection InDirection) const
{
	int32 OriginIndex;
	UOriginShiftTile* Origin = GetOrigin(&OriginIndex);
	FIntVector2 OriginCoordinates = GetTileCoordinates(OriginIndex);

	FIntVector2 Difference(0);
	switch (InDirection)
	{
	case EMapTileDirection::Left:
		Difference = FIntVector2(0, -1);
		break;
		
	case EMapTileDirection::Right:
		Difference = FIntVector2(0, 1);
		break;
		
	case EMapTileDirection::Forward:
		Difference = FIntVector2(1, 0);
		break;
		
	case EMapTileDirection::Backwards:
		Difference = FIntVector2(-1, 0);
		break;

	default:
		checkNoEntry();
		break;
	}

	const FIntVector2 NewOriginCoords = OriginCoordinates + Difference;
	if (NewOriginCoords.X < 0 || NewOriginCoords.X > SideTileLength - 1 || NewOriginCoords.Y < 0 || NewOriginCoords.Y > SideTileLength - 1)
	{
		return false;
	}
	
	Origin->Direction = InDirection;

	UOriginShiftTile* NewOrigin = GetTile(NewOriginCoords);
	NewOrigin->Direction = EMapTileDirection::None;

	return true;
}

void ULimenOriginShiftAlgorithm::ShiftOriginInRandomDirection() const
{
	bool bOriginWasShifted = false;
	TArray<int> Directions = GlobalRandStream->GenerateRandomUniqueNumbers(4, 1, 4);
	for (int Direction : Directions)
	{
		if (bOriginWasShifted = ShiftOrigin(static_cast<EMapTileDirection>(Direction)); bOriginWasShifted)
		{
			AssignTileType();
			break;
		}
	}

#if !NO_LOGGING
	int32 OriginIndex;
	GetOrigin(&OriginIndex);
	const FIntVector2 OriginCoordinates = GetTileCoordinates(OriginIndex);
	
	FString LogMessage = FString::Printf(TEXT("Failed to shift origin at x:%d, y:%d. List of attempts:"), OriginCoordinates.X, OriginCoordinates.Y);
	for (const int32 Direction : Directions)
	{
		LogMessage += FString::Printf(TEXT(" %d"), Direction);
	}

	if (!bOriginWasShifted)
	{
		ULimenCoreStatics::LimenLog(this, LogMessage, ELogType::Error, false);
		checkNoEntry();
	}
#endif
}

void ULimenOriginShiftAlgorithm::AssignTileType() const
{
	for (int i = 0; i < GetGeneratedTileMap()->Count(); ++i)
	{
		UOriginShiftTile* Tile = GetGeneratedTileMap()->GetTile<UOriginShiftTile>(i);
		FTileConnections Connections = GetTileConnections(Tile);

		switch (Connections.Count)
		{
		case 1:
			Tile->Type = ETileType::DeadEnd;
			break;

		case 2:
			if ((Connections.bBack && Connections.bFront) || (Connections.bLeft && Connections.bRight))
			{
				Tile->Type = ETileType::LinearCorridor;
			}
			else
			{
				Tile->Type = ETileType::CurvedCorridor;
			}
			break;

		case 3:
			Tile->Type = ETileType::TJunction;
			break;

		case 4:
			Tile->Type = ETileType::CrossIntersection;
			break;

		default:
			checkNoEntry();
			break;
		}
	}
}

void ULimenOriginShiftAlgorithm::AssignTileRotation() const
{
	Super::AssignTileRotation();
	
	for (auto& Tile : GetGeneratedTileMap()->GetTiles<UOriginShiftTile>())
	{
		const FTileConnections Connections = GetTileConnections(Tile);
		
		FRotator Rotation = FRotator::ZeroRotator;
		
		if (Tile->Type == ETileType::DeadEnd)
		{
			if (Connections.bFront)
			{
				Rotation = FRotator::ZeroRotator;
			}
			else if (Connections.bRight)
			{
				Rotation = FRotator(0.f, 90.f, 0.f);
			}
			else if (Connections.bBack)
			{
				Rotation = FRotator(0.f, 180.f, 0.f);
			}
			else if (Connections.bLeft)
			{
				Rotation = FRotator(0.f, 270.f, 0.f);
			}
			else
			{
				checkNoEntry()
			}
		}
		else if (Tile->Type == ETileType::LinearCorridor)
		{
			if (Connections.bFront && Connections.bBack)
			{
				const double Percentage = GlobalRandStream->GetGlobalRandomStream()->GetFraction() * 100.;
				if (Percentage >= 50.)
				{
					Rotation = FRotator::ZeroRotator;
				}
				else
				{
					Rotation = FRotator(0.f, 180.f, 0.f);
				}
			}
			else if (Connections.bRight && Connections.bLeft)
			{
				if (GlobalRandStream->GetGlobalRandomStream()->GetFraction() >= 0.5f)
				{
					Rotation = FRotator(0.f, 90.f, 0.f);
				}
				else
				{
					Rotation = FRotator(0.f, 270.f, 0.f);
				}
			}
		}
		else if (Tile->Type == ETileType::CurvedCorridor)
		{
			if (Connections.bFront && Connections.bRight)
			{
				Rotation = FRotator::ZeroRotator;
			}
			else if (Connections.bRight && Connections.bBack)
			{
				Rotation = FRotator(0.f, 90.f, 0.f);
			}
			else if (Connections.bBack && Connections.bLeft)
			{
				Rotation = FRotator(0.f, 180.f, 0.f);
			}
			else if (Connections.bLeft && Connections.bFront)
			{
				Rotation = FRotator(0.f, 270.f, 0.f);
			}
		}
		else if (Tile->Type == ETileType::TJunction)
		{
			if (!Connections.bBack)
			{
				Rotation = FRotator::ZeroRotator;
			}
			else if (!Connections.bLeft)
			{
				Rotation = FRotator(0.f, 90.f, 0.f);
			}
			else if (!Connections.bFront)
			{
				Rotation = FRotator(0.f, 180.f, 0.f);
			}
			else if (!Connections.bRight)
			{
				Rotation = FRotator(0.f, 270.f, 0.f);
			}
		}
		else if (Tile->Type == ETileType::CrossIntersection)
		{
			const double Percentage = GlobalRandStream->GetGlobalRandomStream()->GetFraction() * 100.;
			if (Percentage >= 0. && Percentage < 25.)
			{
				Rotation = FRotator(0.f, 0.f, 0.f);
			}
			else if (Percentage >= 25. && Percentage < 50.)
			{
				Rotation = FRotator(0.f, 90.f, 0.f);
			}
			else if (Percentage >= 50. && Percentage < 75.)
			{
				Rotation = FRotator(0.f, 180.f, 0.f);
			}
			else
			{
				Rotation = FRotator(0.f, 270.f, 0.f);
			}
		}
		else
		{
			checkNoEntry()
		}
		
		Tile->Rotation = Rotation;
	}
}

void ULimenOriginShiftAlgorithm::AssignTileClass() const
{
	for (UOriginShiftTile*& Tile : GetGeneratedTileMap()->GetTiles<UOriginShiftTile>())
	{
		const TArray<TSubclassOf<ALimenMapTile>>* RelevantTileList = nullptr;
		
		switch (Tile->Type)
		{
		case ETileType::DeadEnd:
			{
				RelevantTileList = &GetGeneratedTileMapParameters()->GetDeadEndTiles();
			}
			break;

		case ETileType::LinearCorridor:
			{
				RelevantTileList = &GetGeneratedTileMapParameters()->GetLinearCorridorTiles();
			}
			break;

		case ETileType::CurvedCorridor:
			{
				RelevantTileList = &GetGeneratedTileMapParameters()->GetCurvedCorridorTiles();
			}
			break;

		case ETileType::TJunction:
			{
				RelevantTileList = &GetGeneratedTileMapParameters()->GetTJunctionTiles();
			}
			break;

		case ETileType::CrossIntersection:
			{
				RelevantTileList = &GetGeneratedTileMapParameters()->GetCrossIntersectionTiles();
			}
			break;

		default:
			checkNoEntry();
			break;
		}

		check(RelevantTileList != nullptr);
		if (RelevantTileList->IsEmpty()) continue;
		
		const uint64 RandomIndex = GlobalRandStream->GenerateRandomUniqueNumbers(RelevantTileList->Num() - 1, 0, 1)[0];
		Tile->TileClass =(*RelevantTileList)[RandomIndex];
	}
}
