// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ADPCMAudioInfo.h"
#include "ADPCMAudioInfo.h"
#include "LimenProceduralMapBuilder.h"
#include "Interfaces/LimenEndTileInterface.h"
#include "Interfaces/LimenStartTileInterface.h"
#include "LimenProceduralTileMapBuilder.generated.h"


struct FTileInfo;
enum class ETileType : uint8;
class UTileMapDataAsset;
class FThreadedFunction;
class FCheckTileLoadState;
class ALimenMapTile;


UENUM(BlueprintType)
enum class ELimenDirection : uint8
{
	None = 0,
	Left = 1,
	Right = 2,
	Forward = 3,
	Backwards = 4,
};


UCLASS(Blueprintable)
class PROCEDURALMAPS_API ALimenProceduralTileMapBuilder : public ALimenProceduralMapBuilder
{
	GENERATED_BODY()

public:	
	ALimenProceduralTileMapBuilder(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void StartBuildProcess() override;
	
	UFUNCTION(BlueprintCallable, Category="Limen|Procedural Maps|Tiles")
	bool GetTilesByClass(const TSubclassOf<ALimenMapTile> TileClass, TArray<FTileInfo>& OutTiles) const;
	UFUNCTION(BlueprintCallable, Category="Limen|Procedural Maps|Tiles")
	bool TileExistsInLocation(const FVector& Location) const;
	UFUNCTION(BlueprintCallable, Category="Limen|Procedural Maps|Tiles")
	bool TileExistsInDirection(const FVector& Location, const ELimenDirection Direction) const;
	UFUNCTION(BlueprintCallable, Category="Limen|Procedural Maps|Tiles")
	bool GetTileInLocation(const FVector& Location, FTileInfo& OutTileInfo) const;
	UFUNCTION(BlueprintCallable, Category="Limen|Procedural Maps|Tiles")
	bool GetTileInDirection(const FVector& Location, const ELimenDirection Direction, FTileInfo& OutTileInfo) const;
	UFUNCTION(BlueprintCallable, Category="Limen|Procedural Maps|Tiles")
	bool GetClosestTileToLocation(const FVector& Location, FTileInfo& OutTileInfo) const;
	
	virtual void UnloadMap() override;
	virtual void DestroyPreviousMap() override;
	virtual bool SetCurrentMapDataAssetByRef(UProceduralMapDataAsset* MapDataAsset) override;

	UFUNCTION(BlueprintCallable, Category="Limen|Procedural Maps|Tiles")
	TScriptInterface<ILimenStartTileInterface> GetStartTile();

	UFUNCTION(BlueprintCallable, DisplayName="Get End Tile", Category="Limen|Procedural Maps|Tiles")
	bool BP_GetEndTile(ALimenMapTile*& OutTile);
	UFUNCTION(BlueprintCallable, DisplayName="Get End Tile Interface", Category="Limen|Procedural Maps|Tiles")
	TScriptInterface<ILimenEndTileInterface> BP_GetEndTileInterface();
	TScriptInterface<ILimenEndTileInterface> GetEndTile();

	UFUNCTION(BlueprintCallable, Category="Limen|Procedural Maps|Tiles")
	void NotifyPlayerTeleportToStartTile(AController* Controller, APawn* Pawn);

protected:
	FVector CurrentBuildLocation;
	TArray<FTileInfo> MapInfo;
	TArray<FTileInfo> PreviousMapInfo;
	
	TMap<FName, UTileMapDataAsset*> TileMaps;
	TObjectPtr<UTileMapDataAsset> CurrentTileMap;

	virtual void Start() override;
	
	bool CreateTileInfoFromLocation(const FVector& Location, const ELimenDirection& Direction, FTileInfo& OutTileInfo);
	void RemoveTileInDirection(const FVector& Location, const ELimenDirection Direction);
	void RemoveTileInLocation(const FVector& Location);
	bool GetRandomTileInfoIndexOfType(const ETileType TileType, uint64& OutIndex);

	virtual bool CanStart() override;

	UFUNCTION()
	virtual void TileBeginOverlap(AActor* OtherActor);


	
private:	
	/**
	 * @brief The visible tile distance. Greatly decreases draw calls.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Limen|Tiles Settings", meta=(ClampMin="0")) 
	float TileUnloadDistance;

	UPROPERTY(EditDefaultsOnly, Category = "Limen|Tiles Settings", meta=(ClampMin="0")) 
	int64 TileSpawnsPerSecond;

	TSharedPtr<FThreadedFunction> TileLoadStateRunnable;
	TUniquePtr<FRunnableThread> TileLoadStateThread;

	TSharedPtr<FThreadedFunction> MapUnLoaderRunnable;
	TUniquePtr<FRunnableThread> MapUnLoaderThread;
	FCriticalSection MapUnloadingSection;


	bool ValidateTiles() const;

	virtual void GenerateMapLocation_OpenType();
	virtual void GenerateMapLocation_CorridorType();
	void GenerateCorridor();
	void GenerateRooms();
	ELimenDirection CurrentForwardDirection;
	ELimenDirection CurrentBackwardsDirection;
	
	virtual void AssignTilesType();
	virtual void AssignTilesClass();
	
	bool RequiredTilesHaveValidPlace() const;

	void ReplaceTilesWithRequired_OpenType();
	void ReplaceTilesWithRequired_CorridorType();

#pragma region Calculation Steps
	void GenerateMapInfo();
	void ReplaceTilesWithRequired();
	void AssignRotation();
	void RemoveMapInfoDuplicates();
#pragma endregion 

	FTimerHandle TileSpawnTimer;
	int64 LastSpawnedTileIndex;
	UFUNCTION()
	void SpawnTilesPerSecond();
#pragma region Game Thread Steps
	void SpawnTiles();
#pragma endregion

	bool IsBuilderValid();
	static void SeparateTiles(const TArray<TSubclassOf<ALimenMapTile>>& InTiles,
		TArray<TSubclassOf<ALimenMapTile>>* OutDeadEnd, TArray<TSubclassOf<ALimenMapTile>>* OutLinearCorridor,
		TArray<TSubclassOf<ALimenMapTile>>* OutCurvedCorridor, TArray<TSubclassOf<ALimenMapTile>>* OutTJunction,
		TArray<TSubclassOf<ALimenMapTile>>* OutCrossIntersection);
};
