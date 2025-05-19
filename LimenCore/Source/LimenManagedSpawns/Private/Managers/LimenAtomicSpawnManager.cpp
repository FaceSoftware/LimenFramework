// Copyright Face Software. All Rights Reserved.


#include "LimenManagedSpawns/Public/Managers/LimenAtomicSpawnManager.h"

#include "EngineUtils.h"
#include "BlueprintLibraries/LimenCoreStatics.h"
#include "Components/LimenAtomicSpawner.h"


FAtomicSpawnParameters::FAtomicSpawnParameters()
{
	ActorClass = AActor::StaticClass();
	TotalAmount = 1;
	MaxAmountPerSpawner = 1;
	MinAmountPerSpawner = 1;
	SpawnedAmount = 0;

	bUseMaxAmountPerSpawner = true;
	bSnapToFloor = false;
}

ALimenAtomicSpawnManager::ALimenAtomicSpawnManager()
{
}

TArray<AActor*> ALimenAtomicSpawnManager::SpawnItems(const TArray<FAtomicSpawnParameters>& ItemParameters)
{
	TArray<AActor*> Result;
#if WITH_EDITOR
	for (const FAtomicSpawnParameters& Param : ItemParameters)
	{
		if (const TArray<ULimenAtomicSpawner*> ItemSpawners = GetItemSpawners(Param.SpawnTag);
			!CanSpawnAllItems(Param, ItemSpawners))
		{
			const FString LogMessage = FString::Printf(TEXT("Unable to spawn all items of class: %s"), *Param.ActorClass->GetName());
			ULimenCoreStatics::LimenLog(this, LogMessage, ELogType::Error);
			return Result;
		}
	}
#endif

	for (FAtomicSpawnParameters& ItemParam : ULimenCoreStatics::ShuffleArray(ItemParameters))
	{
		TArray<ULimenAtomicSpawner*> Spawners = GetItemSpawners(ItemParam.SpawnTag);
		check(CanSpawnAllItems(ItemParam, Spawners))
		if (Spawners.IsEmpty())
		{
			continue;
		}
		
		TArray<ULimenAtomicSpawner*> ShuffledSpawners = ULimenCoreStatics::ShuffleArray(Spawners);
		while (ItemParam.SpawnedAmount < ItemParam.TotalAmount)
		{
			for (ULimenAtomicSpawner* Spawner : ShuffledSpawners)
			{
				const int32 AmountToSpawn = GetAmountToSpawnForSingleSpawner(ItemParam);
				TArray<AActor*> TempSpawned = Spawner->SpawnItem(ItemParam.ActorClass, AmountToSpawn, ItemParam.bSnapToFloor);

				for (AActor* const& Spawned : TempSpawned)
				{
					if (Spawned)
					{
						ItemParam.SpawnedAmount++;
						Result.Push(Spawned);	
					}
				}
				check(ItemParam.SpawnedAmount <= ItemParam.TotalAmount)
				if (ItemParam.SpawnedAmount == ItemParam.TotalAmount)
				{
					break;
				}
			}
		}
	}

	return Result;
}

TArray<AActor*> ALimenAtomicSpawnManager::SpawnItems()
{
	return SpawnItems(SpawnParameters);
}

TArray<ULimenAtomicSpawner*> ALimenAtomicSpawnManager::GetItemSpawners(const FName& SpawnerTag) const
{
	TArray<ULimenAtomicSpawner*> Spawners;
	Spawners.Reserve(128);

	TActorRange<AActor> Range(GetWorld(), AActor::StaticClass(), EActorIteratorFlags::SkipPendingKill);
	for (const AActor* Actor : Range)
	{
		if (Actor->IsActorBeingDestroyed())
		{
			continue;
		}

		TInlineComponentArray<ULimenAtomicSpawner*> SpawnerComponents;
		Actor->GetComponents<ULimenAtomicSpawner>(SpawnerComponents);

		for (ULimenAtomicSpawner* Spawner : SpawnerComponents)
		{
			if (Spawner != nullptr && Spawner->IsActive() && Spawner->IsTagCompatible(SpawnerTag))
			{
				Spawners.Push(Spawner);
			}
		}
	}
	
	return Spawners;
}

int32 ALimenAtomicSpawnManager::GetAmountToSpawnForSingleSpawner(const FAtomicSpawnParameters& Params)
{
	if (Params.SpawnedAmount >= Params.TotalAmount)
	{
		return 0;
	}

	check(Params.MaxAmountPerSpawner >= Params.MinAmountPerSpawner)

	const int32 MinAmount = Params.MinAmountPerSpawner;
	const int32 MaxAmount = Params.bUseMaxAmountPerSpawner 
		? Params.MaxAmountPerSpawner
		: MinAmount + 1;

	const int32 RandomAmount = ULimenGlobalRandomStreamSubsystem::Get()->RandomIntRange(
		FMath::Max(MinAmount, MaxAmount), 
		FMath::Min(MinAmount, MaxAmount));

	return FMath::Min(RandomAmount, Params.TotalAmount - Params.SpawnedAmount);
}

int32 ALimenAtomicSpawnManager::GetTargetItemCount(const TArray<FAtomicSpawnParameters>& Params)
{
	int32 Target = 0;
	for (const FAtomicSpawnParameters& Param : Params)
	{
		Target += Param.TotalAmount;
	}
	return Target;
}

bool ALimenAtomicSpawnManager::CanSpawnAllItems(const FAtomicSpawnParameters& Param,
                                              const TArray<ULimenAtomicSpawner*>& Spawners)
{
	if (Spawners.IsEmpty())
	{
		return false;
	}
	if (Param.MinAmountPerSpawner > 0)
	{
		return false;
	}

	const float TotalItemsFloat = static_cast<float>(Param.TotalAmount);
	const float SpawnersCountFloat = static_cast<float>(Spawners.Num());
	const int32 ItemsPerSpawner = FMath::CeilToInt(TotalItemsFloat / SpawnersCountFloat);
	if (Param.bUseMaxAmountPerSpawner && ItemsPerSpawner > Param.MaxAmountPerSpawner)
	{
		return false;
	}

	return true;
}
