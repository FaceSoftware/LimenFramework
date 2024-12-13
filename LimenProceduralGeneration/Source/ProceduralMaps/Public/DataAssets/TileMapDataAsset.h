// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ProceduralMapDataAsset.h"
#include "TileMapDataAsset.generated.h"

class ALimenProceduralTileMapBuilder;
class ALimenProceduralMapManager;
class ALimenMapTile;

UENUM(BlueprintType)
enum class ETileMapType : uint8
{
	Open, // Uses random walk algorithm, completely random tile placement
	Corridor // Generates connected corridors and creates rooms on the sides of the corridors. The tile count does not include generated rooms
};

/**
 * 
 */
UCLASS(BlueprintType)
class PROCEDURALMAPS_API UTileMapDataAsset : public UProceduralMapDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Limen|Generation Parameters")
	ETileMapType MapType;

	UPROPERTY(EditAnywhere, Category = "Limen|Generation Parameters", meta=(ClampMin=3, EditCondition="MapType == ETileMapType::Corridor"))
	uint16 MaxCorridorLength;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Limen|Generation Parameters")
	FVector StartPosition;

	UPROPERTY(EditAnywhere, Category = "Limen|Generation Parameters", meta=(ClampMin=2))
	uint64 NumberOfTiles;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Limen|Generation Parameters", meta=(ClampMin=0))
	double TileSize;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Limen|Generation Parameters")
	FRotator DeadEndCompensation;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Limen|Generation Parameters")
	FRotator LinearCorridorCompensation;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Limen|Generation Parameters") 
	FRotator CurvedCorridorCompensation;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Limen|Generation Parameters") 
	FRotator TJunctionCompensation;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Limen|Generation Parameters") 
	FRotator CrossIntersectionCompensation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Limen|Tiles Set")
	TArray<TSubclassOf<ALimenMapTile>> RequiredTilesSet;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Limen|Tiles Set")
	TArray<TSubclassOf<ALimenMapTile>> DeadEnd;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Limen|Tiles Set")
	TArray<TSubclassOf<ALimenMapTile>> LinearCorridor;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Limen|Tiles Set")
	TArray<TSubclassOf<ALimenMapTile>> CurvedCorridor;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Limen|Tiles Set")
	TArray<TSubclassOf<ALimenMapTile>> TJunction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Limen|Tiles Set")
	TArray<TSubclassOf<ALimenMapTile>> CrossIntersection;
};
