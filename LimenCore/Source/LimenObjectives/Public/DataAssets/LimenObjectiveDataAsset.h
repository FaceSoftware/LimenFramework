// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LimenObjectiveDataAsset.generated.h"

struct FObjectiveData;
/**
 * 
 */
UCLASS()
class LIMENOBJECTIVES_API ULimenObjectiveDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly)
	FName UniqueName;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FText Title;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FText Description;
};
