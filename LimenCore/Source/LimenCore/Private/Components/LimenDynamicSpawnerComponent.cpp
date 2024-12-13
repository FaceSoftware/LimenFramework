// Copyright Face Software. All Rights Reserved.


#include "Components/LimenDynamicSpawnerComponent.h"


ULimenDynamicSpawnerComponent::ULimenDynamicSpawnerComponent()
{
	SpawnInterval = 1;
	MaxSpawnRetries = 0;
	MaxSpawns = 20;
}

void ULimenDynamicSpawnerComponent::StartTimer()
{
	checkf(!GetWorld()->GetTimerManager().IsTimerActive(SpawnTimerHandle), TEXT("Cannot start an active timer"));
	GetWorld()->GetTimerManager().SetTimer(SpawnTimerHandle, this, &ThisClass::Wrapper_TrySpawn, SpawnInterval, true);
}

void ULimenDynamicSpawnerComponent::StopTimer()
{
	checkf(GetWorld()->GetTimerManager().IsTimerActive(SpawnTimerHandle), TEXT("Cannot stop an inactive timer"));
	GetWorld()->GetTimerManager().ClearTimer(SpawnTimerHandle);
}

bool ULimenDynamicSpawnerComponent::IsTimerActive() const
{
	return GetWorld()->GetTimerManager().IsTimerActive(SpawnTimerHandle);
}

TArray<TWeakObjectPtr<AActor>> ULimenDynamicSpawnerComponent::GetSpawnedActors()
{
	return SpawnedActors;
}

FActorSpawnParameters ULimenDynamicSpawnerComponent::GetSpawnParameters()
{
	FActorSpawnParameters OutParams;
	OutParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	return OutParams;
}

void ULimenDynamicSpawnerComponent::Wrapper_TrySpawn()
{
	checkf(ClassToSpawn != nullptr, TEXT("Invalid class to spawn"));

	if (SpawnedActors.Num() >= MaxSpawns)
	{
		StopTimer();
	}

	uint16 NumRetries = 0;
	FTransform SpawnTransform;
	do
	{
		if (NumRetries >= MaxSpawnRetries)
		{
			return;
		}
		
		SpawnTransform = GetSpawnTransform();
		NumRetries++;
	}
	while (!ShouldSpawn());

	const FActorSpawnParameters SpawnParameters = GetSpawnParameters();

	const TWeakObjectPtr<AActor> NewActor = GetWorld()->SpawnActor(ClassToSpawn, &SpawnTransform, SpawnParameters);
	check(NewActor != nullptr);
	SpawnedActors.Push(NewActor);
}

/// ULimenRangedDynamicSpawner - Start

ULimenRangedDynamicSpawner::ULimenRangedDynamicSpawner(): MinSpawnRange(0), MaxSpawnRange(0)
{
}

FTransform ULimenRangedDynamicSpawner::GetSpawnTransform()
{
	checkNoEntry();
	return FTransform();
}

void ULimenRangedDynamicSpawner::Wrapper_TrySpawn()
{
	checkf(ClassToSpawn != nullptr, TEXT("Invalid class to spawn"));

	uint16 NumRetries = 0;
	FTransform SpawnTransform;
	do
	{
		if (NumRetries >= MaxSpawnRetries)
		{
			return;
		}

		const FVector SpawnCenter = GetSpawnCenter();
		SpawnTransform = GetSpawnTransform(SpawnCenter);
		NumRetries++;
	}
	while (!ShouldSpawn());

	const FActorSpawnParameters SpawnParameters = GetSpawnParameters();
	
	const TWeakObjectPtr<AActor> NewActor = GetWorld()->SpawnActor(ClassToSpawn, &SpawnTransform, SpawnParameters);
	check(NewActor != nullptr);
	
	SpawnedActors.Push(NewActor);
}

/// ULimenRangedDynamicSpawner - End
