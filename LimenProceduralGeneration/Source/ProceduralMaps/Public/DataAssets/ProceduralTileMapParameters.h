// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ProceduralMapParameters.h"
#include "ProceduralTileMapParameters.generated.h"

class ULimenTileMapAlgorithmBase;
class ALimenProceduralTileMapBuilder;
class ALimenProceduralMapManager;
class ALimenMapTile;


/**
 * 
 */
UCLASS(BlueprintType)
class PROCEDURALMAPS_API UProceduralTileMapParameters : public UProceduralMapParameters
{
	GENERATED_BODY()

public:
	virtual bool ValidateParameters() const override;
	virtual TSubclassOf<ALimenProceduralMapManager> GetManagerClass() const override;
	virtual TSubclassOf<ULimenMapAlgorithm> GetGenerationAlgorithm() const override;
	float GetTileSize() const;
	uint64 GetNumberOfTiles() const;
	FVector GetStartPosition() const;
	const TArray<TSubclassOf<ALimenMapTile>>& GetRequiredTiles() const;
	const TArray<TSubclassOf<ALimenMapTile>>& GetDeadEndTiles() const;
	const TArray<TSubclassOf<ALimenMapTile>>& GetLinearCorridorTiles() const;
	const TArray<TSubclassOf<ALimenMapTile>>& GetCurvedCorridorTiles() const;
	const TArray<TSubclassOf<ALimenMapTile>>& GetTJunctionTiles() const;
	const TArray<TSubclassOf<ALimenMapTile>>& GetCrossIntersectionTiles() const;
	
protected:
	UPROPERTY(EditAnywhere, Category = "Limen")
	TSubclassOf<ALimenProceduralMapManager> MapManagerClass;
	UPROPERTY(EditAnywhere, Category = "Limen")
	TSubclassOf<ULimenMapAlgorithm> GenerationAlgorithm;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Limen|Generation Parameters")
	FVector StartPosition;

	UPROPERTY(EditAnywhere, Category = "Limen|Generation Parameters", meta=(ClampMin=2))
	uint64 NumberOfTiles;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Limen|Generation Parameters", meta=(ClampMin=0))
	double TileSize;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Limen|Tiles Set")
	TArray<TSubclassOf<ALimenMapTile>> RequiredTiles;
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
