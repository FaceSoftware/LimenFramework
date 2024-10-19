// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/LimenGameMenuWidget.h"
#include "LimenInventoryWidget.generated.h"


class ULimenInventoryComponent;
class ALimenItemBase;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FItemActionUIRequest, ULimenItemAction*, ActionRequest);

UCLASS(Abstract, Blueprintable)
class LIMENINTERACTION_API ULimenInventoryWidget : public ULimenGameMenuWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FItemActionUIRequest OnItemActionRequested;

	virtual void NativeConstruct() override;
	
	UFUNCTION(BlueprintCallable)
	void BindInventory(ULimenInventoryComponent* NewBoundInventory);

protected:	
	UFUNCTION(BlueprintCallable, Category="Limen|Inventory Widget")
	ULimenInventoryComponent* GetBoundInventory() const;

	UFUNCTION(BlueprintImplementableEvent)
	void InventoryBound(ULimenInventoryComponent* Inventory);

private:
	UPROPERTY()
	TObjectPtr<ULimenInventoryComponent> BoundInventory;
};
