// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ItemActions/LimenItemAction.h"
#include "LimenHoldItemAction.generated.h"

class ALimenPhysicalItem;
/**
 * 
 */
UCLASS()
class LIMENPLAYERS_API ULimenHoldItemAction : public ULimenItemAction
{
	GENERATED_BODY()

public:
	ULimenHoldItemAction();
	virtual void SetupAction(ALimenItemBase* InItem) override;
	virtual void ActivateAction() override;
	virtual bool CanPerformAction() const override;

private:
	UPROPERTY()
	TObjectPtr<ALimenPhysicalItem> PhysicalItem;
};
