// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LimenDynamicSpawnerComponent.generated.h"


/**
 * 
 */
UCLASS(Abstract)
class LIMENCORE_API ULimenDynamicSpawnerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	ULimenDynamicSpawnerComponent();
	
	void StartTimer();
	void StopTimer();
	bool IsTimerActive() const;

	TArray<TWeakObjectPtr<AActor>> GetSpawnedActors();

protected:
	UPROPERTY(EditDefaultsOnly, Category="Limen")
	TSubclassOf<AActor> ClassToSpawn;
	UPROPERTY(EditDefaultsOnly, Category="Limen")
	float SpawnInterval;
	UPROPERTY(EditDefaultsOnly, Category="Limen", meta=(ClampMin="1"))
	int32 MaxSpawns;
	/**
	 * @brief Attempts to find a valid spawn location. -1 means try until it succeeds.
	 */
	UPROPERTY(EditDefaultsOnly, Category="Limen", meta=(ClampMin="-1"))
	int32 MaxSpawnRetries;

	UPROPERTY()
	TArray<TWeakObjectPtr<AActor>> SpawnedActors;
	
	virtual bool ShouldSpawn() PURE_VIRTUAL(ULimenDynamicSpawnerComponent::ShouldSpawn, return {};);
	virtual FTransform GetSpawnTransform() PURE_VIRTUAL(ULimenDynamicSpawnerComponent::GetSpawnTransform, return {};);
	virtual FActorSpawnParameters GetSpawnParameters();

private:
	FTimerHandle SpawnTimerHandle;

	virtual void Wrapper_TrySpawn();
};

UCLASS(Abstract)
class LIMENCORE_API ULimenRangedDynamicSpawner : public ULimenDynamicSpawnerComponent
{
	GENERATED_BODY()

public:
	ULimenRangedDynamicSpawner();
	
protected:
	UPROPERTY(EditDefaultsOnly, Category="Limen")
	float MinSpawnRange;
	UPROPERTY(EditDefaultsOnly, Category="Limen")
	float MaxSpawnRange;

	virtual FTransform GetSpawnTransform() override;
	virtual FTransform GetSpawnTransform(const FVector& SpawnCenter) PURE_VIRTUAL(ULimenDynamicSpawnerComponent::GetSpawnTransform, return {};);
	virtual FVector GetSpawnCenter() PURE_VIRTUAL(ULimenRangedDynamicSpawner::GetSpawnCenter, return FVector(););

private:
	virtual void Wrapper_TrySpawn() override;
};
