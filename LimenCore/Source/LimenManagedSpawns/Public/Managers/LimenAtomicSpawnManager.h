// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actors/LimenGameplayManager.h"
#include "LimenAtomicSpawnManager.generated.h"


class ALimenAtomicSpawnManager;
class ULimenAtomicSpawner;

USTRUCT()
struct LIMENMANAGEDSPAWNS_API FAtomicSpawnParameters
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="Limen")
	TSubclassOf<AActor> ActorClass;

	UPROPERTY(EditAnywhere, Category="Limen")
	FName SpawnTag;
	
	UPROPERTY(EditAnywhere, Category="Limen", meta=(ClampMin="1"))
	int32 TotalAmount;
	
	UPROPERTY(EditAnywhere, Category="Limen", meta=(ClampMin="1"))
	int32 MaxAmountPerSpawner;
	
	UPROPERTY(EditAnywhere, Category="Limen", meta=(ClampMin="1"))
	int32 MinAmountPerSpawner;

	FAtomicSpawnParameters();

private:
	friend ALimenAtomicSpawnManager;

	int32 SpawnedAmount;
};

UCLASS()
class LIMENMANAGEDSPAWNS_API ALimenAtomicSpawnManager : public ALimenGameplayManager
{
	GENERATED_BODY()

public:
	ALimenAtomicSpawnManager();
	
	void SpawnItems(const TArray<FAtomicSpawnParameters>& ItemParameters);

private:	
	TArray<ULimenAtomicSpawner*> GetItemSpawners(const FName& SpawnerTag) const;
	static int32 GetAmountToSpawnForSingleSpawner(const FAtomicSpawnParameters& Params);
	static int32 GetTargetItemCount(const TArray<FAtomicSpawnParameters>& Params);
	static bool CanSpawnAllItems(const FAtomicSpawnParameters& Param, const TArray<ULimenAtomicSpawner*>& Spawners);
};
