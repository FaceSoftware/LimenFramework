// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DataAssets/LimenItemDataAsset.h"
#include "DataAssets/LimenItemDataAsset.h"
#include "LimenWeaponDataAsset.generated.h"


UCLASS()
class LIMENWEAPONS_API ULimenWeaponDataAsset : public ULimenItemDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, meta=(ClampMin="0"))
	float BaseDamage;

	UPROPERTY(EditDefaultsOnly, meta=(ClampMin="0", ClampMax="100"))
	float ImpactDamageFalloffPercent;
	
	UPROPERTY(EditDefaultsOnly, meta=(ClampMin="0"))
	float WeaponRange;
	
	UPROPERTY(EditDefaultsOnly, meta=(ClampMin="0"))
	float RoundsPerSecond;

	UPROPERTY(EditDefaultsOnly, meta=(ClampMin="1"))
	uint8 MagazineCapacity;
	
	UPROPERTY(EditDefaultsOnly, meta=(ClampMin="0"))
	float ReloadTimeInSeconds;

	UPROPERTY(EditDefaultsOnly)
	bool bIsAutomatic;

	UPROPERTY(EditDefaultsOnly, meta=(ClampMin="0"))
	int CurrentAmmo;

	UPROPERTY(EditDefaultsOnly)
	FName TargetAmmoUniqueName;
};
