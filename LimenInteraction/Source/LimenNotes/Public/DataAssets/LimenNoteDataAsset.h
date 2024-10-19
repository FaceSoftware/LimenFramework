// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DataAssets/LimenItemDataAsset.h"
#include "LimenNoteDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class LIMENNOTES_API ULimenNoteDataAsset : public ULimenItemDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	FText NoteText;
	
};
