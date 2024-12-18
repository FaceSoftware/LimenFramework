// Copyright Face Software. All Rights Reserved.


#include "Components/LimenUpgradeManager.h"

#include "Components/LimenCreditsComponent.h"
#include "DataAssets/LimenUpgradeDataAsset.h"
#include "Interfaces/LimenUpgradable.h"


ULimenUpgradeManager::ULimenUpgradeManager()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void ULimenUpgradeManager::OnComponentCreated()
{
	Super::OnComponentCreated();
	
	// Check for duplicates and assert if one has been found
	for (int32 i = 0; i < UpgradesData.Num(); ++i)
	{
		TObjectPtr<ULimenUpgradeDataAsset> Upgrade = UpgradesData[i].LoadSynchronous();

#if !UE_BUILD_SHIPPING
		if (!ensureMsgf(IsValid(Upgrade), TEXT("Empty upgrade in upgrades list")))
		{
			return;
		}
#endif
		
		Upgrade->CurrentLevel = Upgrade->StartLevel;
		
#if !UE_BUILD_SHIPPING
		for (int32 j = 0; j < UpgradesData.Num(); ++j)
		{
			if (i == j)
			{
				continue;
			}

			if (!ensureMsgf(UpgradesData[i] != UpgradesData[j], TEXT("Duplicate found in upgrades list")))
			{
				return;
			}
		}
#endif
	}
}

void ULimenUpgradeManager::BeginPlay()
{
	Super::BeginPlay();

	check(GetOwner()->Implements<ULimenUpgradable>());
}

const TArray<TSoftObjectPtr<ULimenUpgradeDataAsset>>& ULimenUpgradeManager::GetUpgradesData() const
{
	return UpgradesData;
}

void ULimenUpgradeManager::Upgrade(APlayerController* Controller, APawn* Pawn, ULimenUpgradeDataAsset* Upgrade)
{	
	const EUpgradeResponse Response = CanUpgrade(Controller, Pawn, Upgrade);
	if (Response != EUpgradeResponse::Success)
	{
		OnUpgradeProcessed.Broadcast(this, Response);
		return;
	}
	
	if (ULimenCreditsComponent* CreditsComponent = Pawn->GetComponentByClass<ULimenCreditsComponent>())
	{
		check(CreditsComponent->HasCredits(Upgrade->LevelsCost[Upgrade->CurrentLevel]));
		CreditsComponent->RemoveCredits(Upgrade->LevelsCost[Upgrade->CurrentLevel]);
	}
	
	ILimenUpgradable::Execute_Upgrade(GetOwner(), ++Upgrade->CurrentLevel, Upgrade);
	OnUpgradeProcessed.Broadcast(this, Response);
}

EUpgradeResponse ULimenUpgradeManager::CanUpgrade(APlayerController* Controller, APawn* Pawn,
												  const ULimenUpgradeDataAsset* Upgrade)
{
	if (!IsValid(Upgrade))
	{
		return EUpgradeResponse::UpgradeNotAvailable;
	}
	
	const ULimenCreditsComponent* CreditsComponent = Pawn->GetComponentByClass<ULimenCreditsComponent>();
	if (!CreditsComponent)
	{
		// Defaults to success for better compatibility (it depends on the implementation, credits may not even be used)
		return EUpgradeResponse::Success;
	}

	if (Upgrade->CurrentLevel >= static_cast<uint32>(Upgrade->LevelsCost.Num()))
	{
		return EUpgradeResponse::MaxLevelReached;
	}
	
	if (!CreditsComponent->HasCredits(Upgrade->LevelsCost[Upgrade->CurrentLevel]))
	{
		return EUpgradeResponse::NotEnoughCredits;
	}
	
	return EUpgradeResponse::Success;
}

int32 ULimenUpgradeManager::GetCurrentUpgradeLevel(ULimenUpgradeDataAsset* Upgrade) const
{
	check(Upgrade);
	check(UpgradesData.Find(Upgrade) != INDEX_NONE);
	return Upgrade->CurrentLevel;
}

int32 ULimenUpgradeManager::GetCurrentUpgradeLevel(const TSoftObjectPtr<ULimenUpgradeDataAsset>& Upgrade) const
{
	check(!Upgrade.IsNull());
	return GetCurrentUpgradeLevel(Upgrade.LoadSynchronous());
}

float ULimenUpgradeManager::GetNextUpgradeCost(ULimenUpgradeDataAsset* Upgrade) const
{
	check(IsValid(Upgrade));
	check(UpgradesData.Find(Upgrade) != INDEX_NONE);
	if (!Upgrade->LevelsCost.IsValidIndex(Upgrade->CurrentLevel))
	{
		return -1;
	}
	
	return Upgrade->LevelsCost[Upgrade->CurrentLevel];
}

bool ULimenUpgradeManager::IsAtMaxLevel(const ULimenUpgradeDataAsset* Upgrade) const
{
	check(IsValid(Upgrade))
	return Upgrade->CurrentLevel >= static_cast<uint32>(Upgrade->LevelsCost.Num());
}
