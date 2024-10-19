// Copyright Face Software. All Rights Reserved.


#include "Components/LimenItemSpawner.h"

#include "Items/LimenItemBase.h"
#include "BlueprintLibraries/LimenCoreStatics.h"
#include "Subsystems/LimenGlobalRandomStreamSubsystem.h"


ULimenItemSpawner::ULimenItemSpawner()
{
	PrimaryComponentTick.bCanEverTick = false;
	
	ItemSpawnerParameters.ChanceToSpawn = 50.f;
	ItemSpawnerParameters.MaxItemsToSpawn = 1;
	ItemSpawnerParameters.bAllowNoSpawn = true;
}

void ULimenItemSpawner::SpawnItems()
{
	if (ItemSpawnerParameters.ItemClasses.IsEmpty())
	{
		return;
	}
	
	auto* GlobalRandomStream = ULimenGlobalRandomStreamSubsystem::Get();
	check(GlobalRandomStream)

	if (ItemSpawnerParameters.bAllowNoSpawn)
	{
		bool bWon;
		ULimenCoreStatics::RandomPercentage(ItemSpawnerParameters.ChanceToSpawn, bWon);
		if (!bWon)
		{
			return;
		}
	}

	const uint16 NumberOfItems = GlobalRandomStream->RandomIntRange(ItemSpawnerParameters.MaxItemsToSpawn, 1);
	check(NumberOfItems > 0)
	
	TArray<int> ItemIndexes = GlobalRandomStream->GenerateRandomNumbers(ItemSpawnerParameters.ItemClasses.Num() - 1, 0, NumberOfItems);
	check(!ItemIndexes.IsEmpty())

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	for (const auto& ItemIndex : ItemIndexes)
	{
		FVector SpawnLocation = GetComponentLocation();
		auto* Item = GetWorld()->SpawnActor<ALimenItemBase>(ItemSpawnerParameters.ItemClasses[ItemIndex], SpawnLocation, FRotator(0), SpawnParams);
		SpawnedItems.Push(Item);
	}
}

void ULimenItemSpawner::SetItemClasses(const TArray<TSubclassOf<ALimenItemBase>>& InItemClasses)
{
	ItemSpawnerParameters.ItemClasses = InItemClasses;
}

void ULimenItemSpawner::AddItemToSpawn(const TSubclassOf<ALimenItemBase>& InItem)
{
	ItemSpawnerParameters.ItemClasses.Push(InItem);
}

void ULimenItemSpawner::SetSpawnerParameters(const FItemSpawnerParameters& InParams)
{
	ItemSpawnerParameters = InParams;
}

TArray<ALimenItemBase*> ULimenItemSpawner::GetSpawnedItems() const
{
	return SpawnedItems;
}
