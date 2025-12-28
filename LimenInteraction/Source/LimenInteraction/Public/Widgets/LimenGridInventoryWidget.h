// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Widget.h"
#include "LimenGridInventoryWidget.generated.h"


class ULimenGridInventoryComponent;
class SLimenGridInventory;

/**
 * 
 */
UCLASS()
class LIMENINTERACTION_API ULimenGridInventoryWidget : public UWidget
{
	GENERATED_BODY()
	
public:
	ULimenGridInventoryWidget();
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
	
	UFUNCTION(BlueprintCallable)
	void BindInventory(ULimenGridInventoryComponent* InInventory);
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Grid Inventory")
	FSlateBrush CellBackgroundBrush;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Grid Inventory")
	FSlateBrush InvalidPlacementHighlightBrush;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Grid Inventory")
	FSlateBrush ValidPlacementHighlightBrush;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Grid Inventory")
	FVector2D CellSize;

private:
	TSharedPtr<SLimenGridInventory> GridInventory;
	TWeakObjectPtr<ULimenGridInventoryComponent> GridInventoryComponent;
};
