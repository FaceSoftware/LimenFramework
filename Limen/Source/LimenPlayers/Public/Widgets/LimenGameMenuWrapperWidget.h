// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/LimenInventoryWidget.h"
#include "Widgets/LimenMenuWrapperWidget.h"
#include "LimenGameMenuWrapperWidget.generated.h"


class ULimenItemAction;
class ULimenObjectiveInventory;
enum class EItemAction : uint8;
class ALimenItemBase;
class ALimenPlayerController;

/**
 * Base class for the GameMenu. It works as a wrapper for the inventory menu, objectives menu, etc.
 * This should include a WidgetSwitcher to change between menus.
 */
UCLASS(Abstract, Blueprintable)
class LIMENPLAYERS_API ULimenGameMenuWrapperWidget : public ULimenMenuWrapperWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintCallable)
	FItemActionUIRequest OnItemActionRequest;
	
protected:
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<ULimenObjectiveInventory> ObjectivesMenuWidget;
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<ULimenInventoryWidget> InventoryMenuWidget;

	virtual void ReturnToGame() override;
	
	virtual void OnWidgetHidden_Implementation() override;
};
