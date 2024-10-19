// Copyright Face Software. All Rights Reserved.


#include "Actors/LimenProceduralTileMapBuilder.h"

#include "Subsystems/LimenGlobalRandomStreamSubsystem.h"
#include "BlueprintLibraries/LimenCoreStatics.h"
#include "Components/LimenDynamicLoadingComponent.h"
#include "DataAssets/TileMapDataAsset.h"
#include "Interfaces/LimenStartTileInterface.h"
#include "Interfaces/LimenEndTileInterface.h"
#include "Threading/ThreadedFunction.h"
#include "Threading/ThreadingStatics.h"
#include "Actors/LimenMapTile.h"
#include "DataAssets/ProceduralMapDataAsset.h"
#include "Managers/LimenProceduralMapManager.h"


ALimenProceduralTileMapBuilder::ALimenProceduralTileMapBuilder(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bRunOnAnyThread = true;

	CurrentForwardDirection = ELimenDirection::None;
	CurrentBackwardsDirection = ELimenDirection::None;
	
	CurrentMapIndex = -1;
	TileUnloadDistance = 5000.f;
	TileSpawnsPerSecond = 20;
	LastSpawnedTileIndex = 0;

	if (IsValid(GetWorld()) && GetWorld()->IsGameWorld())
	{
		ADD_CALCULATION_STEP(this, &ThisClass::GenerateMapInfo)
		ADD_CALCULATION_STEP(this, &ThisClass::ReplaceTilesWithRequired)
		ADD_CALCULATION_STEP(this, &ThisClass::AssignRotation)
		ADD_CALCULATION_STEP(this, &ThisClass::RemoveMapInfoDuplicates)
		
		ADD_GAMETHREAD_STEP(this, &ThisClass::SpawnTiles)
	}
}

void ALimenProceduralTileMapBuilder::StartBuildProcess()
{
	if (!IsBuilderValid() || !ValidateTiles())
	{
		return;
	}
	
	if (!MapInfo.IsEmpty())
	{
		UnloadMap();
	}
	
	Super::StartBuildProcess();

	OnMapStartedBuilding.Broadcast(CurrentMapIndex);
}

bool ALimenProceduralTileMapBuilder::GetTilesByClass(const TSubclassOf<ALimenMapTile> TileClass,
                                                     TArray<FTileInfo>& OutTiles) const
{
	for (const auto& Tile : MapInfo)
	{
		if (Tile.TileClass == TileClass)
		{
			OutTiles.Push(Tile);
		}
	}

	return !OutTiles.IsEmpty();
}

bool ALimenProceduralTileMapBuilder::CreateTileInfoFromLocation(const FVector& Location, const ELimenDirection& Direction,
                                                                FTileInfo& OutTileInfo)
{
	FVector BuildLocation = Location;

	switch (Direction)
	{
	case ELimenDirection::Left:
		BuildLocation -= FVector(0.f, CurrentTileMap->TileSize, 0.f);
		break;

	case ELimenDirection::Right:
		BuildLocation += FVector(0.f, CurrentTileMap->TileSize, 0.f);
		break;

	case ELimenDirection::Forward:
		BuildLocation += FVector(CurrentTileMap->TileSize, 0.f, 0.f);
		break;

	case ELimenDirection::Backwards:
		BuildLocation -= FVector(CurrentTileMap->TileSize, 0.f, 0.f);
		break;

	default:
		checkNoEntry()
		break;
	}

	FTileInfo NewTile;
	NewTile.Location = BuildLocation;

	OutTileInfo = NewTile;
	return true;
}

bool ALimenProceduralTileMapBuilder::TileExistsInLocation(const FVector& Location) const
{
	for (const auto& TileInfo : MapInfo)
	{
		if (TileInfo.Location == Location)
		{
			return true;
		}
	}

	return false;
}

bool ALimenProceduralTileMapBuilder::GetTileInLocation(const FVector& Location, FTileInfo& OutTileInfo) const
{
	for (const auto& TileInfo : MapInfo)
	{
		if (TileInfo.Location == Location)
		{
			OutTileInfo = TileInfo;
			return true;
		}
	}

	return false;
}

void ALimenProceduralTileMapBuilder::RemoveTileInLocation(const FVector& Location)
{
	for (int i = 0; i < MapInfo.Num(); ++i)
	{
		if (MapInfo[i].Location == Location)
		{
			MapInfo.RemoveAt(i);
			return;
		}
	}
}

bool ALimenProceduralTileMapBuilder::TileExistsInDirection(const FVector& Location, const ELimenDirection Direction) const
{
	FVector LocationToCheck = Location;

	switch (Direction)
	{
	case ELimenDirection::Left:
		LocationToCheck -= FVector(0.f, CurrentTileMap->TileSize, 0.f);
		break;

	case ELimenDirection::Right:
		LocationToCheck += FVector(0.f, CurrentTileMap->TileSize, 0.f);
		break;

	case ELimenDirection::Forward:
		LocationToCheck += FVector(CurrentTileMap->TileSize, 0.f, 0.f);
		break;

	case ELimenDirection::Backwards:
		LocationToCheck -= FVector(CurrentTileMap->TileSize, 0.f, 0.f);
		break;

	default:
		return false;
	}

	return TileExistsInLocation(LocationToCheck);
}

bool ALimenProceduralTileMapBuilder::GetTileInDirection(const FVector& Location, const ELimenDirection Direction,
                                                        FTileInfo& OutTileInfo) const
{
	FVector LocationToCheck = Location;

	switch (Direction)
	{
	case ELimenDirection::Left:
		LocationToCheck -= FVector(0.f, CurrentTileMap->TileSize, 0.f);
		break;

	case ELimenDirection::Right:
		LocationToCheck += FVector(0.f, CurrentTileMap->TileSize, 0.f);
		break;

	case ELimenDirection::Forward:
		LocationToCheck += FVector(CurrentTileMap->TileSize, 0.f, 0.f);
		break;

	case ELimenDirection::Backwards:
		LocationToCheck -= FVector(CurrentTileMap->TileSize, 0.f, 0.f);
		break;

	default:
		return false;
	}

	return GetTileInLocation(LocationToCheck, OutTileInfo);
}

bool ALimenProceduralTileMapBuilder::GetClosestTileToLocation(const FVector& Location, FTileInfo& OutTileInfo) const
{
	if (MapInfo.IsEmpty())
	{
		return false;
	}

	float MinDistance = std::numeric_limits<float>::max();
	bool bFound = false;

	for (const FTileInfo& Tile : MapInfo)
	{
		const float CurrentDistance = FVector::DistSquared(Location, Tile.Location);

		if (CurrentDistance < MinDistance)
		{
			MinDistance = CurrentDistance;
			OutTileInfo = Tile;
			bFound = true;
		}
	}

	return bFound;
}

bool ALimenProceduralTileMapBuilder::SetCurrentMapDataAssetByRef(UProceduralMapDataAsset* MapDataAsset)
{
	if (!Super::SetCurrentMapDataAssetByRef(MapDataAsset))
	{
		return false;
	}
	
	UTileMapDataAsset* TileMapData = Cast<UTileMapDataAsset>(MapDataAsset);
	if (!IsValid(TileMapData))
	{
		ULimenCoreStatics::LimenLog(this, TEXT("Invalid map data"), ELogType::Error);
		return false;
	}
	
	CurrentTileMap = TileMapData;
	return true;
}

TScriptInterface<ILimenStartTileInterface> ALimenProceduralTileMapBuilder::GetStartTile()
{
	for (const auto& TileInfo : MapInfo)
	{
		check(TileInfo.TileActor)
		if (TileInfo.TileActor->Implements<ULimenStartTileInterface>())
		{
			return TileInfo.TileActor.Get();
		}
	}
	
	return nullptr;
}

bool ALimenProceduralTileMapBuilder::BP_GetEndTile(ALimenMapTile*& OutTile)
{
	for (const auto& TileInfo : MapInfo)
	{
		if (TileInfo.TileActor == nullptr)
		{
			continue;
		}
		
		if (TileInfo.TileActor->Implements<ULimenEndTileInterface>())
		{
			OutTile = TileInfo.TileActor.Get();
			return true;
		}
	}

	return false;
}

TScriptInterface<ILimenEndTileInterface> ALimenProceduralTileMapBuilder::BP_GetEndTileInterface()
{
	return GetEndTile();
}

TScriptInterface<ILimenEndTileInterface> ALimenProceduralTileMapBuilder::GetEndTile()
{
	ALimenMapTile* Out = nullptr;
	return BP_GetEndTile(Out) ? TScriptInterface<ILimenEndTileInterface>(Out) : TScriptInterface<ILimenEndTileInterface>(nullptr);
}

void ALimenProceduralTileMapBuilder::NotifyPlayerTeleportToStartTile(AController* Controller, APawn* Pawn)
{
	UObject* Start = GetStartTile().GetObject();
	if (!IsValid(Start))
	{
		return;
	}

	ILimenStartTileInterface::Execute_PlayerTeleported(Start, Controller, Pawn);
}

void ALimenProceduralTileMapBuilder::Start()
{
	Super::Start();
	
	TileLoadStateRunnable = MakeShared<FThreadedFunction>([this]
	{
		for (const FTileInfo& Tile : MapInfo)
		{
			if (Tile.TileActor)
			{
				Tile.TileActor->CheckTileLoadState();
			}
		}
	});
	
		MapUnLoaderRunnable = MakeShared<FThreadedFunction>([this]
		{
			AsyncTask(ENamedThreads::GameThread, [this]
			{
	
			});
		});
}

void ALimenProceduralTileMapBuilder::RemoveTileInDirection(const FVector& Location, const ELimenDirection Direction)
{
	FVector LocationToCheck = Location;

	switch (Direction)
	{
	case ELimenDirection::Left:
		LocationToCheck -= FVector(0.f, CurrentTileMap->TileSize, 0.f);
		break;

	case ELimenDirection::Right:
		LocationToCheck += FVector(0.f, CurrentTileMap->TileSize, 0.f);
		break;

	case ELimenDirection::Forward:
		LocationToCheck += FVector(CurrentTileMap->TileSize, 0.f, 0.f);
		break;

	case ELimenDirection::Backwards:
		LocationToCheck -= FVector(CurrentTileMap->TileSize, 0.f, 0.f);
		break;

	default:
		checkNoEntry()
		break;
	}

	RemoveTileInLocation(LocationToCheck);
}

bool ALimenProceduralTileMapBuilder::GetRandomTileInfoIndexOfType(const ETileType TileType, uint64& OutIndex)
{	
	TArray<uint64> CompatibleTiles;
	for (int i = 0; i < MapInfo.Num(); i++)
	{
		if (TileType == MapInfo[i].Type)
		{
			CompatibleTiles.Add(i);
		}
	}

	if (CompatibleTiles.IsEmpty())
	{
		return false;
	}

	auto RandomIndex = ULimenGlobalRandomStreamSubsystem::Get()->GenerateRandomUniqueNumbers(CompatibleTiles.Num() - 1, 0, 1);
	OutIndex = CompatibleTiles[RandomIndex[0]];
	return true;
}

bool ALimenProceduralTileMapBuilder::CanStart()
{
	return Super::CanStart();
}

void ALimenProceduralTileMapBuilder::TileBeginOverlap(AActor* OtherActor)
{
	if (Cast<APawn>(OtherActor))
	{
		if (TileLoadStateThread)
		{
			TileLoadStateThread->Kill(true);	
			TileLoadStateThread.Reset();
		}
		
		TileLoadStateThread = FThreadingStatics::MakeRunnableThread(TileLoadStateRunnable.ToSharedRef(), TEXT("TileLoadStateCheck"));
	}
}

bool ALimenProceduralTileMapBuilder::ValidateTiles() const
{
	bool bValid = true;
	for (const auto& Tile : CurrentTileMap->DeadEnd)
	{
		if (Tile.GetDefaultObject()->GetTileType() != ETileType::DeadEnd)
		{
			ULimenCoreStatics::LimenLog(this, TEXT("A tile that's not a dead end is in the dead end tile set"), ELogType::Error);
			bValid = false;
		}
	}

	for (const auto& Tile : CurrentTileMap->LinearCorridor)
	{
		if (Tile.GetDefaultObject()->GetTileType() != ETileType::LinearCorridor)
		{
			ULimenCoreStatics::LimenLog(this, TEXT("A tile that's not a linear corridor is in the linear corridor tile set"), ELogType::Error);
		}
	}

	for (const auto& Tile : CurrentTileMap->CurvedCorridor)
	{
		if (Tile.GetDefaultObject()->GetTileType() != ETileType::CurvedCorridor)
		{
			ULimenCoreStatics::LimenLog(this, TEXT("A tile that's not a linear corridor is in the curved corridor tile set"), ELogType::Error);
		}
	}
	
	for (const auto& Tile : CurrentTileMap->TJunction)
	{
		if (Tile.GetDefaultObject()->GetTileType() != ETileType::TJunction)
		{
			ULimenCoreStatics::LimenLog(this, TEXT("A tile that's not a TJunction is in the TJunction tile set"), ELogType::Error);
		}
	}

	for (const auto& Tile : CurrentTileMap->CrossIntersection)
	{
		if (Tile.GetDefaultObject()->GetTileType() != ETileType::CrossIntersection)
		{
			ULimenCoreStatics::LimenLog(this, TEXT("A tile that's not a cross intersection is in the cross intersection tile set"), ELogType::Error);
		}
	}

	return bValid;
}

void ALimenProceduralTileMapBuilder::GenerateMapLocation_OpenType()
{
	CurrentBuildLocation = CurrentTileMap->StartPosition;
	for (uint64 CurrentTileNum = 0; CurrentTileNum < CurrentTileMap->NumberOfTiles ; ++CurrentTileNum)
	{
		const ELimenDirection Direction = static_cast<ELimenDirection>(GetRandomStream()->RandRange(1, 4));
		
		FTileInfo NewTileInfo;
		const bool bSuccess = CreateTileInfoFromLocation(CurrentBuildLocation, Direction, NewTileInfo);
		check(bSuccess)

		CurrentBuildLocation = NewTileInfo.Location;

		if (TileExistsInLocation(NewTileInfo.Location))
		{
			// Retry while there's not an available position.
			CurrentTileNum--;
			continue;
		}

		MapInfo.Push(NewTileInfo);
	}
}

void ALimenProceduralTileMapBuilder::GenerateMapLocation_CorridorType()
{
	do
	{
		GenerateCorridor();
	}
	while (MapInfo.Num() < CurrentTileMap->NumberOfTiles);
	GenerateRooms();
}

void ALimenProceduralTileMapBuilder::GenerateCorridor()
{
	while (true)
	{
		if (MapInfo.IsEmpty())
		{
			CurrentBuildLocation = CurrentTileMap->StartPosition;
		}
		else
		{
			// Randomize the map by picking a random tile as the initial position for the corridor
			ULimenGlobalRandomStreamSubsystem* GlobalRandomStream = ULimenGlobalRandomStreamSubsystem::Get();
			check(GlobalRandomStream)

			const int32 RandomTileIndex = GlobalRandomStream->GenerateRandomUniqueNumbers(MapInfo.Num() - 1, 0, 1)[0];
			CurrentBuildLocation = MapInfo[RandomTileIndex].Location;
			check(TileExistsInLocation(CurrentBuildLocation))
		}

		CurrentForwardDirection = static_cast<ELimenDirection>(GetRandomStream()->RandRange(1, 4));
		if (!TileExistsInDirection(CurrentBuildLocation, CurrentForwardDirection))
		{
			break;
		}
	}
	
	for (uint64 CurrentTileNum = 0; CurrentTileNum < CurrentTileMap->MaxCorridorLength; ++CurrentTileNum)
	{
		if (CurrentTileNum > 0 &&
			TileExistsInDirection(CurrentBuildLocation, ELimenDirection::Left) +
			TileExistsInDirection(CurrentBuildLocation, ELimenDirection::Forward) +
			TileExistsInDirection(CurrentBuildLocation, ELimenDirection::Right) +
			TileExistsInDirection(CurrentBuildLocation, ELimenDirection::Backwards) > 1)
		{
			return;
		}

		FTileInfo NewTileInfo;
		const bool bSuccess = CreateTileInfoFromLocation(CurrentBuildLocation, CurrentForwardDirection, NewTileInfo);
		check(bSuccess)

		CurrentBuildLocation = NewTileInfo.Location;
		MapInfo.Push(NewTileInfo);

		if (MapInfo.Num() >= CurrentTileMap->NumberOfTiles)
		{
			return;
		}
	}
}

void ALimenProceduralTileMapBuilder::GenerateRooms()
{
	auto* GlobalRandomStream = ULimenGlobalRandomStreamSubsystem::Get();
	check(GlobalRandomStream)

	uint64 RequiredDeadEnds = CurrentTileMap->DeadEnd.Num();
	for (const auto RequiredTile : CurrentTileMap->RequiredTilesSet)
	{
		if (RequiredTile->GetDefaultObject<ALimenMapTile>()->GetTileType() == ETileType::DeadEnd)
		{
			RequiredDeadEnds++;
		}
	}

	TArray<int> Indexes = GlobalRandomStream->GenerateValidRandomUniqueNumbers(MapInfo.Num() - 1, 0, RequiredDeadEnds,
	[this, GlobalRandomStream](const int TestIndex) -> bool
	{
		TArray<ELimenDirection> FreeDirections;
		if (!TileExistsInDirection(MapInfo[TestIndex].Location, ELimenDirection::Backwards))
		{
			FreeDirections.Push(ELimenDirection::Backwards);
		}
		if (!TileExistsInDirection(MapInfo[TestIndex].Location, ELimenDirection::Forward))
		{
			FreeDirections.Push(ELimenDirection::Forward);
		}
		if (!TileExistsInDirection(MapInfo[TestIndex].Location, ELimenDirection::Left))
		{
			FreeDirections.Push(ELimenDirection::Left);
		}
		if (!TileExistsInDirection(MapInfo[TestIndex].Location, ELimenDirection::Right))
		{
			FreeDirections.Push(ELimenDirection::Right);
		}

		// We only care about tiles where it's possible to attach another one
		if (FreeDirections.IsEmpty())
		{
			return false;
		}

		const uint8 RandomFreeDirectionIndex = GlobalRandomStream->GenerateRandomUniqueNumbers(FreeDirections.Num() - 1, 0, 1)[0];
		FTileInfo RoomTile;
		verify(CreateTileInfoFromLocation(MapInfo[TestIndex].Location, FreeDirections[RandomFreeDirectionIndex], RoomTile))
		RoomTile.bIsRoom = true;

		// Add the room
		MapInfo.Push(RoomTile);
		
		return true;
	});
}

void ALimenProceduralTileMapBuilder::AssignTilesType()
{
	for (auto& Tile : MapInfo)
	{		
		const bool Backwards = TileExistsInDirection(Tile.Location, ELimenDirection::Backwards);
		const bool Forward = TileExistsInDirection(Tile.Location, ELimenDirection::Forward);
		const bool Left = TileExistsInDirection(Tile.Location, ELimenDirection::Left);
		const bool Right = TileExistsInDirection(Tile.Location, ELimenDirection::Right);
		
		switch (Backwards + Forward + Left + Right)
		{
		case 1:
			Tile.Type = ETileType::DeadEnd;
			break;

		case 2:
			if (Backwards && Forward || Left && Right)
			{
				Tile.Type = ETileType::LinearCorridor;
			}
			else
			{
				Tile.Type = ETileType::CurvedCorridor;
			}

			break;

		case 3:
			Tile.Type = ETileType::TJunction;
			break;

		case 4:
			Tile.Type = ETileType::CrossIntersection;
			break;

		default:
			checkNoEntry()
			break;
		}
	}
}

void ALimenProceduralTileMapBuilder::AssignTilesClass()
{
	for (auto& Tile : MapInfo)
	{
		uint64 RandomIndex;
		switch (Tile.Type)
		{
		case ETileType::DeadEnd:
			RandomIndex = ULimenGlobalRandomStreamSubsystem::Get()->GenerateRandomUniqueNumbers(
				CurrentTileMap->DeadEnd.Num() - 1, 0, 1)[0];
			Tile.TileClass = CurrentTileMap->DeadEnd[RandomIndex];
			break;

		case ETileType::LinearCorridor:
			RandomIndex = ULimenGlobalRandomStreamSubsystem::Get()->GenerateRandomUniqueNumbers(
				CurrentTileMap->LinearCorridor.Num() - 1, 0, 1)[0];
			Tile.TileClass = CurrentTileMap->LinearCorridor[RandomIndex];
			break;

		case ETileType::CurvedCorridor:
			RandomIndex = ULimenGlobalRandomStreamSubsystem::Get()->GenerateRandomUniqueNumbers(
				CurrentTileMap->CurvedCorridor.Num() - 1, 0, 1)[0];
			Tile.TileClass = CurrentTileMap->CurvedCorridor[RandomIndex];
			break;

		case ETileType::TJunction:
			RandomIndex = ULimenGlobalRandomStreamSubsystem::Get()->GenerateRandomUniqueNumbers(
				CurrentTileMap->TJunction.Num() - 1, 0, 1)[0];
			Tile.TileClass = CurrentTileMap->TJunction[RandomIndex];
			break;

		case ETileType::CrossIntersection:
			RandomIndex = ULimenGlobalRandomStreamSubsystem::Get()->GenerateRandomUniqueNumbers(
				CurrentTileMap->CrossIntersection.Num() - 1, 0, 1)[0];
			Tile.TileClass = CurrentTileMap->CrossIntersection[RandomIndex];
			break;

		default:
			checkNoEntry()
			break;
		}
	}
}

bool ALimenProceduralTileMapBuilder::RequiredTilesHaveValidPlace() const
{
	TArray<TSubclassOf<ALimenMapTile>> RequiredDeadEndTiles;
	TArray<TSubclassOf<ALimenMapTile>> RequiredLinearCorridorTiles;
	TArray<TSubclassOf<ALimenMapTile>> RequiredCurvedCorridorTiles;
	TArray<TSubclassOf<ALimenMapTile>> RequiredTJunctionTiles;
	TArray<TSubclassOf<ALimenMapTile>> RequiredCrossIntersectionTiles;
	SeparateTiles(CurrentTileMap->RequiredTilesSet, &RequiredDeadEndTiles, &RequiredLinearCorridorTiles, &RequiredCurvedCorridorTiles, &RequiredTJunctionTiles, &RequiredCrossIntersectionTiles);

	uint32 Dead = 0, Linear = 0, Curved = 0, T = 0, Cross = 0;
	const uint32 RequiredDead = RequiredDeadEndTiles.Num();
	const uint32 RequiredLinear = RequiredLinearCorridorTiles.Num();
	const uint32 RequiredCurved = RequiredCurvedCorridorTiles.Num();
	const uint32 RequiredT = RequiredTJunctionTiles.Num();
	const uint32 RequiredCross = RequiredCrossIntersectionTiles.Num();
	
	for (const auto RequiredTile : MapInfo)
	{
		const auto Class = RequiredTile.TileClass;
		if (Class->GetDefaultObject<ALimenMapTile>()->GetTileType() == ETileType::CrossIntersection)
		{
			Cross++;
		}
		else if (Class->GetDefaultObject<ALimenMapTile>()->GetTileType() == ETileType::TJunction)
		{
			T++;
		}
		else if (Class->GetDefaultObject<ALimenMapTile>()->GetTileType() == ETileType::LinearCorridor)
		{
			Linear++;
		}
		else if (Class->GetDefaultObject<ALimenMapTile>()->GetTileType() == ETileType::CurvedCorridor)
		{
			Curved++;
		}
		else if (Class->GetDefaultObject<ALimenMapTile>()->GetTileType() == ETileType::DeadEnd)
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

void ALimenProceduralTileMapBuilder::ReplaceTilesWithRequired_OpenType()
{
	if (CurrentTileMap->RequiredTilesSet.IsEmpty())
	{
		return;
	}

	if (CurrentTileMap->NumberOfTiles < CurrentTileMap->RequiredTilesSet.Num())
	{
		const FString Message = TEXT("Number of iterations must be greater (or equal) than RequiredTilesSet. Otherwise, all required tiles may not be included. The value has been changed");
		ULimenCoreStatics::LimenLog(this, Message, ELogType::Error);

		CurrentTileMap->NumberOfTiles = CurrentTileMap->RequiredTilesSet.Num();
	}

	TArray<uint64> UsedIndexes;
	for (int i = 0; i < CurrentTileMap->RequiredTilesSet.Num(); ++i)
	{
		bool bIsValidIndex = true;
		uint64 TileIndex = 0;
		if (!GetRandomTileInfoIndexOfType(CurrentTileMap->RequiredTilesSet[i]->GetDefaultObject<ALimenMapTile>()->GetTileType(), TileIndex))
		{
			const FString Message = TEXT("Unable to find a valid location for a required tile. Try changing the seed or add more iterations. Otherwise, map may be incomplete.");
			ULimenCoreStatics::LimenLog(this, Message, ELogType::Error);
			continue;
		}

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
		
		MapInfo[TileIndex].TileClass = CurrentTileMap->RequiredTilesSet[i];
		UsedIndexes.Push(TileIndex);
	}
}

void ALimenProceduralTileMapBuilder::ReplaceTilesWithRequired_CorridorType()
{
	if (CurrentTileMap->RequiredTilesSet.IsEmpty())
	{
		return;
	}

	if (CurrentTileMap->NumberOfTiles < CurrentTileMap->RequiredTilesSet.Num())
	{
		const FString Message = TEXT("Number of iterations must be greater (or equal) than RequiredTilesSet. Otherwise, all required tiles may not be included. The value has been changed");
		ULimenCoreStatics::LimenLog(this, Message, ELogType::Error);

		CurrentTileMap->NumberOfTiles = CurrentTileMap->RequiredTilesSet.Num();
	}

	// Search for non dead ends, since dead ends should be treated as rooms
	TArray<uint64> UsedIndexes;
	UsedIndexes.Reserve(CurrentTileMap->RequiredTilesSet.Num());
	
	for (int i = 0; i < CurrentTileMap->RequiredTilesSet.Num(); ++i)
	{
		if (CurrentTileMap->RequiredTilesSet[i]->GetDefaultObject<ALimenMapTile>()->GetTileType() == ETileType::DeadEnd)
		{
			continue;
		}
		
		bool bIsValidIndex = true;
		uint64 TileIndex = 0;
		if (!GetRandomTileInfoIndexOfType(CurrentTileMap->RequiredTilesSet[i].GetDefaultObject()->GetTileType(), TileIndex))
		{
			const FString Message = TEXT("Unable to find a valid location for a required tile. Try changing the seed or add more iterations. Otherwise, map may be incomplete.");
			ULimenCoreStatics::LimenLog(this, Message, ELogType::Error);
			continue;
		}

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
		
		MapInfo[TileIndex].TileClass = CurrentTileMap->RequiredTilesSet[i];
		UsedIndexes.Push(TileIndex);
	}

	// Check for dead ends
	uint64 NumRequiredDeadEnds = 0;
	for (int i = 0; i < CurrentTileMap->RequiredTilesSet.Num(); ++i)
	{
		if (CurrentTileMap->RequiredTilesSet[i]->GetDefaultObject<ALimenMapTile>()->GetTileType() == ETileType::DeadEnd)
		{
			NumRequiredDeadEnds++;
		}
	}

	if (NumRequiredDeadEnds == 0)
	{
		return;
	}

	auto* GlobalRandomStream = ULimenGlobalRandomStreamSubsystem::Get();
	check(GlobalRandomStream)
	const TArray<int> RandomRoomIndexes = GlobalRandomStream->GenerateValidRandomUniqueNumbers(MapInfo.Num() - 1, 0, NumRequiredDeadEnds,
		[this] (const int Index)
		{
			if (MapInfo[Index].bIsRoom)
			{
				// Only dead ends are considered as rooms
				check(MapInfo[Index].Type == ETileType::DeadEnd)

				return true;
			}

			return false;
		});

	for (int i = 0; i < CurrentTileMap->RequiredTilesSet.Num(); i++)
	{
		check(RandomRoomIndexes.IsValidIndex(i))
		check(CurrentTileMap->RequiredTilesSet.IsValidIndex(i))
		
		MapInfo[RandomRoomIndexes[i]].TileClass = CurrentTileMap->RequiredTilesSet[i];
	}
}

void ALimenProceduralTileMapBuilder::GenerateMapInfo()
{
	do
	{
		// We need to empty the map info array, otherwise if the call to RequiredTilesHaveValidPlace returns false,
		// It will reset and the previous information will persist, creating a map with more tiles than it should
		MapInfo.Empty();

		switch (CurrentTileMap->MapType)
		{
		case ETileMapType::Open:
			GenerateMapLocation_OpenType();
			check(MapInfo.Num() == CurrentTileMap->NumberOfTiles)
			break;
			
		case ETileMapType::Corridor:
			GenerateMapLocation_CorridorType();
			// This map generation method generated the required number of and adds the required dead ends as rooms
			
#if WITH_EDITOR
			uint64 RequiredDeadEnds = CurrentTileMap->DeadEnd.Num();
			for (const auto RequiredTile : CurrentTileMap->RequiredTilesSet)
			{
				if (RequiredTile->GetDefaultObject<ALimenMapTile>()->GetTileType() == ETileType::DeadEnd)
				{
					RequiredDeadEnds++;
				}
			}
			check(MapInfo.Num() == CurrentTileMap->NumberOfTiles + RequiredDeadEnds)
#endif
			
			break;
		}
		
		AssignTilesType();
		AssignTilesClass();
	}
	while (!RequiredTilesHaveValidPlace() || ShouldStopMapBuilderThread());
}

void ALimenProceduralTileMapBuilder::ReplaceTilesWithRequired()
{	
	switch (CurrentTileMap->MapType)
	{
	case ETileMapType::Open:
		ReplaceTilesWithRequired_OpenType();
		break;
		
	case ETileMapType::Corridor:
		ReplaceTilesWithRequired_OpenType();
		// ReplaceTilesWithRequired_CorridorType();
		break;
		
	default:
		break;
	}}

void ALimenProceduralTileMapBuilder::AssignRotation()
{	
	for (auto& Tile : MapInfo)
	{
		FTileInfo BackwardsTile;
		const bool Backwards = TileExistsInDirection(Tile.Location, ELimenDirection::Backwards);
		FTileInfo ForwardTile;
		const bool Forward = TileExistsInDirection(Tile.Location, ELimenDirection::Forward);
		FTileInfo LeftTile;
		const bool Left = TileExistsInDirection(Tile.Location, ELimenDirection::Left);
		FTileInfo RightTile;
		const bool Right = TileExistsInDirection(Tile.Location, ELimenDirection::Right);

		FRotator Rotation = FRotator::ZeroRotator;
		
		if (Tile.Type == ETileType::DeadEnd)
		{
			if (Forward && ForwardTile.Type != ETileType::DeadEnd)
			{
				Rotation = FRotator::ZeroRotator + CurrentTileMap->DeadEndCompensation;
			}
			else if (Right && RightTile.Type != ETileType::DeadEnd)
			{
				Rotation = FRotator(0.f, 90.f, 0.f) + CurrentTileMap->DeadEndCompensation;
			}
			else if (Backwards && BackwardsTile.Type != ETileType::DeadEnd)
			{
				Rotation = FRotator(0.f, 180.f, 0.f) + CurrentTileMap->DeadEndCompensation;
			}
			else if (Left && LeftTile.Type != ETileType::DeadEnd)
			{
				Rotation = FRotator(0.f, 270.f, 0.f) + CurrentTileMap->DeadEndCompensation;
			}
			else
			{
				checkNoEntry()
			}
		}
		else if (Tile.TileClass->GetDefaultObject<ALimenMapTile>()->GetTileType() == ETileType::LinearCorridor)
		{
			if (Forward && Backwards)
			{
				const double Percentage = GetRandomStream()->GetFraction() * 100.;
				if (Percentage >= 50.)
				{
					Rotation = FRotator::ZeroRotator + CurrentTileMap->LinearCorridorCompensation;
				}
				else
				{
					Rotation = FRotator(0.f, 180.f, 0.f) + CurrentTileMap->LinearCorridorCompensation;
				}
			}
			else if (Right && Left)
			{
				if (GetRandomStream()->GetFraction() >= 0.5f)
				{
					Rotation = FRotator(0.f, 90.f, 0.f) + CurrentTileMap->LinearCorridorCompensation;
				}
				else
				{
					Rotation = FRotator(0.f, 270.f, 0.f) + CurrentTileMap->LinearCorridorCompensation;
				}
			}
		}
		else if (Tile.TileClass->GetDefaultObject<ALimenMapTile>()->GetTileType() == ETileType::CurvedCorridor)
		{
			if (Forward && Right)
			{
				Rotation = FRotator::ZeroRotator + CurrentTileMap->CurvedCorridorCompensation;
			}
			else if (Right && Backwards)
			{
				Rotation = FRotator(0.f, 90.f, 0.f) + CurrentTileMap->CurvedCorridorCompensation;
			}
			else if (Backwards && Left)
			{
				Rotation = FRotator(0.f, 180.f, 0.f) + CurrentTileMap->CurvedCorridorCompensation;
			}
			else if (Left && Forward)
			{
				Rotation = FRotator(0.f, 270.f, 0.f) + CurrentTileMap->CurvedCorridorCompensation;
			}
		}
		else if (Tile.TileClass->GetDefaultObject<ALimenMapTile>()->GetTileType() == ETileType::TJunction)
		{
			if (!Backwards)
			{
				Rotation = FRotator::ZeroRotator + CurrentTileMap->TJunctionCompensation;
			}
			else if (!Left)
			{
				Rotation = FRotator(0.f, 90.f, 0.f) + CurrentTileMap->TJunctionCompensation;
			}
			else if (!Forward)
			{
				Rotation = FRotator(0.f, 180.f, 0.f) + CurrentTileMap->TJunctionCompensation;
			}
			else if (!Right)
			{
				Rotation = FRotator(0.f, 270.f, 0.f) + CurrentTileMap->TJunctionCompensation;
			}
		}
		else if (Tile.TileClass->GetDefaultObject<ALimenMapTile>()->GetTileType() == ETileType::CrossIntersection)
		{
			const double Percentage = GetRandomStream()->GetFraction() * 100.;
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
		
		Tile.Rotation = Rotation.Clamp();
	}
}

void ALimenProceduralTileMapBuilder::RemoveMapInfoDuplicates()
{
	for (int i = 0; i < MapInfo.Num(); ++i)
	{
		for (int j = 0; j < MapInfo.Num(); ++j)
		{
			if (MapInfo[i].Location == MapInfo[j].Location && i != j)
			{
				MapInfo.RemoveAt(j, 1, true);
				// If the array was shrunk,
				// the j index has a different element
				// and j is decremented so we don't overlook it.
				j--;
			}
		}
	}
}

void ALimenProceduralTileMapBuilder::SpawnTilesPerSecond()
{
	uint64 CurrentRepetitions = 0;
	// Todo: CanCreateTile is not used but this while doesn't make much sense. Needs changes!!
	while (!MapInfo[LastSpawnedTileIndex].TileClass->GetDefaultObject<ALimenMapTile>()->CanCreateTile(MapInfo[LastSpawnedTileIndex], this))
	{
		CurrentRepetitions++;
		checkf(CurrentRepetitions >= 1000, TEXT("Failed to create tile after 1000 retries"))
	}

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	MapInfo[LastSpawnedTileIndex].TileActor = GetWorld()->SpawnActor<ALimenMapTile>(MapInfo[LastSpawnedTileIndex].TileClass,
																					MapInfo[LastSpawnedTileIndex].Location,
																					MapInfo[LastSpawnedTileIndex].Rotation);
	
	check(IsValid(MapInfo[LastSpawnedTileIndex].TileActor))
	
	MapInfo[LastSpawnedTileIndex].TileActor->GetDynamicLoadingComponent()->SetTileUnloadDistance(TileUnloadDistance);
	MapInfo[LastSpawnedTileIndex].TileActor->OnBoundingBoxBeginOverlap.AddUniqueDynamic(this, &ThisClass::TileBeginOverlap);
	MapInfo[LastSpawnedTileIndex].TileActor->Initialize();

	MapInfo[LastSpawnedTileIndex].TileActor->AssignedTileInfo = MapInfo[LastSpawnedTileIndex];

	// If this is the last index, stop the timer and notify the builder thread
	if (LastSpawnedTileIndex == MapInfo.Num() - 1)
	{
		GetWorld()->GetTimerManager().ClearTimer(TileSpawnTimer);

		// Complete the promise that the game thread operation is finished
		TPromise<bool>& Successful = GetGameThreadStageCompletePromise();
		Successful.SetValue(true);
				
		return;
	}
	
	LastSpawnedTileIndex++;
}

void ALimenProceduralTileMapBuilder::SpawnTiles()
{
	LastSpawnedTileIndex = 0;
	GetWorld()->GetTimerManager().ClearTimer(TileSpawnTimer);
	GetWorld()->GetTimerManager().SetTimer(TileSpawnTimer, this, &ALimenProceduralTileMapBuilder::SpawnTilesPerSecond, 1.f / TileSpawnsPerSecond, true, 0.f);
}

bool ALimenProceduralTileMapBuilder::IsBuilderValid()
{	
	bool bIsValid = true;
	if (CurrentTileMap->CrossIntersection.IsEmpty())
	{
		bIsValid = false;
		const FString Message = TEXT("CrossIntersection tile set cannot be empty");
		ULimenCoreStatics::LimenLog(this, Message, ELogType::Error);
	}

	if (CurrentTileMap->TJunction.IsEmpty())
	{
		bIsValid = false;
		const FString Message = TEXT("TJunction tile set cannot be empty");
		ULimenCoreStatics::LimenLog(this, Message, ELogType::Error);
	}

	if (CurrentTileMap->LinearCorridor.IsEmpty())
	{
		bIsValid = false;
		const FString Message = TEXT("LinearCorridor tile set cannot be empty");
		ULimenCoreStatics::LimenLog(this, Message, ELogType::Error);
	}

	if (CurrentTileMap->CurvedCorridor.IsEmpty())
	{
		bIsValid = false;
		const FString Message = TEXT("CurvedCorridor tile set cannot be empty");
		ULimenCoreStatics::LimenLog(this, Message, ELogType::Error);
	}

	if (CurrentTileMap->DeadEnd.IsEmpty())
	{
		bIsValid = false;
		const FString Message = TEXT("DeadEnd tile set cannot be empty");
        ULimenCoreStatics::LimenLog(this, Message, ELogType::Error);
	}

	if (CurrentTileMap->RequiredTilesSet.Num() > CurrentTileMap->NumberOfTiles)
	{
		bIsValid = false;
		const FString Message = TEXT("Number of iterations cannot be lower than the RequiredTilesSet count");
		ULimenCoreStatics::LimenLog(this, Message, ELogType::Error);
	}

	TArray<UProceduralMapDataAsset*> MapData;
	Maps.GenerateValueArray(MapData);
	for (auto* Map : MapData)
	{
		const FName* MapName = Maps.FindKey(Map);
		
		UTileMapDataAsset* TileMapData = Cast<UTileMapDataAsset>(Map);		
		if (!IsValid(TileMapData))
		{
			bIsValid = false;
			const FString Message = FString::Printf(TEXT("Invalid map data found for map with name: %s"), *MapName->ToString());
			ULimenCoreStatics::LimenLog(this, Message, ELogType::Error);
			continue;
		}
		
		TileMaps.Add(*MapName, TileMapData);
	}

	bIsValid &= CurrentTileMap->MaxCorridorLength >= 3;

	return bIsValid;
}

void ALimenProceduralTileMapBuilder::UnloadMap()
{
	Super::UnloadMap();

	if (MapInfo.IsEmpty())
	{
		return;
	}
	
	PreviousMapInfo = MapInfo;
	MapInfo.Empty();
	
	if (TileLoadStateThread.IsValid())
	{
		TileLoadStateThread->Kill(true);
		TileLoadStateThread.Reset();
	}
}

void ALimenProceduralTileMapBuilder::DestroyPreviousMap()
{
	Super::DestroyPreviousMap();
	
	for (auto& Tile : PreviousMapInfo)
	{
		if (Tile.TileActor)
		{
			verify(Tile.TileActor->Destroy());
			Tile.TileActor = nullptr;
		}
	}
	PreviousMapInfo.Empty();
}

void ALimenProceduralTileMapBuilder::SeparateTiles(const TArray<TSubclassOf<ALimenMapTile>>& InTiles,
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
