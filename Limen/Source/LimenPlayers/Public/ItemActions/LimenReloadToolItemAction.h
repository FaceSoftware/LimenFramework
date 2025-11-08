// Copyright © 2024 FaceSoftware. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ItemActions/LimenItemAction.h"
#include "LimenReloadToolItemAction.generated.h"


class ULimenInventoryComponent;
class ALimenTool;

/**
 * 
 */
UCLASS()
class UE_DEPRECATED(5.6, "") LIMENPLAYERS_API ULimenReloadToolItemAction : public ULimenItemAction
{
	GENERATED_BODY()

public:
	ULimenReloadToolItemAction();
	virtual void SetupAction(ALimenItemBase* InItem) override;
   	virtual void ActivateAction() override;
   	virtual bool CanPerformAction() const override;
    
private:
   	UPROPERTY()
   	TObjectPtr<ALimenTool> Tool;
};
