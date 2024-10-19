// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LimenInteractableDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class LIMENINTERACTION_API ULimenInteractableDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName UniqueName;
};
