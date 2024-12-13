// Copyright Face Software. All Rights Reserved.


#include "LimenManagedSpawns/Public/Managers/LimenAtomicSpawnManager.h"

#include "EngineUtils.h"
#include "BlueprintLibraries/LimenCoreStatics.h"
#include "LimenManagedSpawns/Public/Components/LimenAtomicSpawner.h"
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
		const TArray<ULimenAtomicSpawner*> ItemSpawners = GetItemSpawners(Param.SpawnTag);
		if (!CanSpawnAllItems(Param, ItemSpawners))
		{
			const FString LogMessage = FString::Printf(TEXT("Unable to spawn all items of class: %s"), *Param.ActorClass->GetName());
			ULimenCoreStatics::LimenLog(this, LogMessage, ELogType::Warning);
		}
	}

#endif
	
	TArray<FAtomicSpawnParameters> ShuffledItemParams = ULimenCoreStatics::ShuffleArray(ItemParameters);

	const int32 TargetItemCount = GetTargetItemCount(ShuffledItemParams);
	int32 TotalItemCount = 0;
	int32 SpawnerIndex = 0;
	
	while (TotalItemCount < TargetItemCount)
	{
		for (FAtomicSpawnParameters& ItemParam : ShuffledItemParams)
		{
			const TArray<ULimenAtomicSpawner*> ShuffledItemSpawners = ULimenCoreStatics::ShuffleArray(GetItemSpawners(ItemParam.SpawnTag));
			check(!ShuffledItemSpawners.IsEmpty())

			const int32 AmountToSpawn = GetAmountToSpawnForSingleSpawner(ItemParam);

			ShuffledItemSpawners[SpawnerIndex]->SpawnItem(ItemParam.ActorClass, AmountToSpawn);
			TotalItemCount += AmountToSpawn;
			ItemParam.SpawnedAmount += AmountToSpawn;
			
			if (SpawnerIndex + 1 >= ShuffledItemSpawners.Num())
			{
				SpawnerIndex = 0;
			}
			else
			{
				SpawnerIndex++;
			}
		}
	}
	
	LIMEN_LOG(LogLimen, Log, this, "Spawned %d items", TotalItemCount);
}

TArray<ULimenAtomicSpawner*> ALimenAtomicSpawnManager::GetItemSpawners(const FName& SpawnerTag) const
{
	TArray<ULimenAtomicSpawner*> Spawners;
	Spawners.Reserve(128);

	for (TActorIterator<AActor> It(GetWorld()); It; ++It)
	{
		for (TSet<UActorComponent*> Components = It->GetComponents(); UActorComponent* Component : Components)
		{
			ULimenAtomicSpawner* Spawner = Cast<ULimenAtomicSpawner>(Component);
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
	const int32 RandomAmount = ULimenGlobalRandomStreamSubsystem::Get()->RandomIntRange(Params.MaxAmountPerSpawner,
																						Params.MinAmountPerSpawner);

	if (RandomAmount + Params.SpawnedAmount >= Params.TotalAmount)
	{
		return Params.TotalAmount - Params.SpawnedAmount;
	}

	return RandomAmount;
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
	
	return (Param.TotalAmount / Spawners.Num()) <= Param.MaxAmountPerSpawner;
}
