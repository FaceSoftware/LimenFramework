// Copyright FaceSoftware. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/LimenGameMenuWidget.h"
#include "LimenItemSelectionWidget.generated.h"


class ALimenItemBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FItemSelectedDelegate, ALimenItemBase*, item);


/**
 * 
 */
UCLASS()
class LIMENINTERACTION_API ULimenItemSelectionWidget : public ULimenGameMenuWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FItemSelectedDelegate OnItemSelected;
	
	UFUNCTION(BlueprintCallable)
	void SetAvailableItemOptions(const TArray<TSubclassOf<ALimenItemBase>>& InOptions);
	UFUNCTION(BlueprintCallable)
	void NotifyItemSelected(ALimenItemBase* Item);

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void AvailableItemsUpdated(const TArray<TSubclassOf<ALimenItemBase>>& ItemClasses);

private:
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	TArray<TSubclassOf<ALimenItemBase>> Options;
};
