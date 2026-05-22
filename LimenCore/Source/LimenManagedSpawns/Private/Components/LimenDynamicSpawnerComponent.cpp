// Copyright Face Software. All Rights Reserved.


#include "Components/LimenDynamicSpawnerComponent.h"

#include "NavigationSystem.h"
#include "Kismet/KismetMathLibrary.h"


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

bool ULimenDynamicSpawnerComponent::IsActorSet() const
{
	return ClassToSpawn != nullptr;
}

void ULimenDynamicSpawnerComponent::DespawnAllActors()
{
	for (TWeakObjectPtr<AActor>& Actor : GetSpawnedActors())
	{
		if (Actor.IsValid())
		{
			Actor->Destroy();
		}
	}
}

TArray<TWeakObjectPtr<AActor>> ULimenDynamicSpawnerComponent::GetSpawnedActors()
{
	return SpawnedActors;
}

bool ULimenDynamicSpawnerComponent::ShouldSpawn()
{
	return true;
}

bool ULimenDynamicSpawnerComponent::GetSpawnTransform(FTransform& OutTransform)
{
	OutTransform = FTransform::Identity;
	return true;
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

		if (!GetSpawnTransform(SpawnTransform))
		{
			NumRetries++;
		}
	}
	while (!ShouldSpawn());

	const FActorSpawnParameters SpawnParameters = GetSpawnParameters();

	const TWeakObjectPtr<AActor> NewActor = GetWorld()->SpawnActor(ClassToSpawn, &SpawnTransform, SpawnParameters);
	check(NewActor != nullptr);
	SpawnedActors.Push(NewActor);
}

#pragma region ULimenRangedDynamicSpawner

ULimenRangedDynamicSpawner::ULimenRangedDynamicSpawner()
{
	MinSpawnRange = 0;
	MaxSpawnRange = 0;
}

bool ULimenRangedDynamicSpawner::GetSpawnTransform(FTransform& OutTransform)
{
	const UNavigationSystemV1* NavSystem = CastChecked<UNavigationSystemV1>(GetWorld()->GetNavigationSystem());

	const FVector Center = GetSpawnCenter();
	FNavLocation OutLocation;
	if (!NavSystem->GetRandomReachablePointInRadius(Center, MaxSpawnRange, OutLocation))
	{
		return false;
	}

	OutTransform.SetLocation(OutLocation.Location);
	OutTransform.SetRotation(UKismetMathLibrary::FindLookAtRotation(OutLocation.Location, Center).Quaternion());
	OutTransform.SetScale3D(FVector::OneVector);

	return true;
}

FVector ULimenRangedDynamicSpawner::GetSpawnCenter()
{
	return FVector::ZeroVector;
}

#pragma endregion ULimenRangedDynamicSpawner
