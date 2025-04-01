// Copyright Face Software. All Rights Reserved.


#include "LimenManagedSpawns/Public/Managers/LimenAtomicSpawnManager.h"

#include "EngineUtils.h"
#include "BlueprintLibraries/LimenCoreStatics.h"
#include "Components/LimenAtomicSpawner.h"
#include "LogMacros/LimenLogMacros.h"


FAtomicSpawnParameters::FAtomicSpawnParameters()
{
	ActorClass = AActor::StaticClass();
	TotalAmount = 1;
	MaxAmountPerSpawner = 1;
	MinAmountPerSpawner = 1;
	SpawnedAmount = 0;
}

ALimenAtomicSpawnManager::ALimenAtomicSpawnManager()
{
}

void ALimenAtomicSpawnManager::SpawnItems(const TArray<FAtomicSpawnParameters>& ItemParameters)
{
#if WITH_EDITOR
	for (const FAtomicSpawnParameters& Param : ItemParameters)
	{
		if (const TArray<ULimenAtomicSpawner*> ItemSpawners = GetItemSpawners(Param.SpawnTag);
			!CanSpawnAllItems(Param, ItemSpawners))
		{
			const FString LogMessage = FString::Printf(TEXT("Unable to spawn all items of class: %s"), *Param.ActorClass->GetName());
			ULimenCoreStatics::LimenLog(this, LogMessage, ELogType::Error);
			return;
		}
	}
#endif

	for (FAtomicSpawnParameters& ItemParam : ULimenCoreStatics::ShuffleArray(ItemParameters))
	{
		TArray<ULimenAtomicSpawner*> ShuffledSpawners = ULimenCoreStatics::ShuffleArray(GetItemSpawners(ItemParam.SpawnTag));

		while (ItemParam.SpawnedAmount < ItemParam.TotalAmount)
		{
			for (ULimenAtomicSpawner* Spawner : ShuffledSpawners)
			{
				const int32 AmountToSpawn = GetAmountToSpawnForSingleSpawner(ItemParam);
				Spawner->SpawnItem(ItemParam.ActorClass, AmountToSpawn);

				ItemParam.SpawnedAmount += AmountToSpawn;
				check(ItemParam.SpawnedAmount <= ItemParam.TotalAmount)

				if (ItemParam.SpawnedAmount == ItemParam.TotalAmount)
				{
					break;
				}
			}
		}
	}
}

TArray<ULimenAtomicSpawner*> ALimenAtomicSpawnManager::GetItemSpawners(const FName& SpawnerTag) const
{
	TArray<ULimenAtomicSpawner*> Spawners;
	Spawners.Reserve(128);

	for (const AActor* Actor : TActorRange<AActor>(GetWorld()))
	{
		if (Actor->IsActorBeingDestroyed())
		{
			continue;
		}

		TInlineComponentArray<ULimenAtomicSpawner*> SpawnerComponents;
		Actor->GetComponents<ULimenAtomicSpawner>(SpawnerComponents);

		for (ULimenAtomicSpawner* Spawner : SpawnerComponents)
		{
			if (Spawner != nullptr && Spawner->IsTagCompatible(SpawnerTag))
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
	const int32 RandomAmount = ULimenGlobalRandomStreamSubsystem::Get()->RandomIntRange(Params.MaxAmountPerSpawner,
																						Params.MinAmountPerSpawner);

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
	if (Param.MinAmountPerSpawner > 0 && Spawners.Num() > Param.TotalAmount)
	{
		return false;
	}

	const float TotalItemsFloat = static_cast<float>(Param.TotalAmount);
	const float SpawnersCountFloat = static_cast<float>(Spawners.Num());
	return FMath::CeilToInt(TotalItemsFloat / SpawnersCountFloat) <= Param.MaxAmountPerSpawner;
}
