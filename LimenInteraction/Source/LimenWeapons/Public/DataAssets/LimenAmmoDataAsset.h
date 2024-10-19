// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DataAssets/LimenItemDataAsset.h"
#include "LimenAmmoDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class LIMENWEAPONS_API ULimenAmmoDataAsset : public ULimenItemDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	int AmmoQuantity;
};
