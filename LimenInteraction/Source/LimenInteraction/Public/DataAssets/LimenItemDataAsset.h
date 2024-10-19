// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LimenInteractableDataAsset.h"
#include "LimenItemDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class LIMENINTERACTION_API ULimenItemDataAsset : public ULimenInteractableDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UTexture2D> ItemImage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText DisplayName = FText::FromString(L"Display Name");
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Description = FText::FromString(L"Description");
	/**
	 * @brief The name of the action that this item can perform (egg. Note - 'Read', Flashlight - 'Hold')
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText UniqueActionName;
	/**
	 * @brief Can separate instances have different properties and should occupy different inventory slots?
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bEachIsUnique = false;
};
