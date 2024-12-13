// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ItemActions/LimenItemAction.h"
#include "LimenHolsterItemAction.generated.h"

class ALimenPhysicalItem;
/**
 * 
 */
UCLASS()
class LIMENPLAYERS_API ULimenHolsterItemAction : public ULimenItemAction
{
	GENERATED_BODY()

public:
	ULimenHolsterItemAction();
	virtual void SetupAction(ALimenItemBase* InItem) override;
	virtual void ActivateAction() override;
	virtual bool CanPerformAction() const override;

private:
	UPROPERTY()
	TObjectPtr<ALimenPhysicalItem> PhysicalItem;
};
