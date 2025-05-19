// Copyright FaceSoftware. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Buttons/LimenSelectableMenuButton.h"
#include "Components/Widget.h"
#include "LimenTabMenu.generated.h"

class ULimenStandardButton;
class ULimenSelectableMenuButton;


/**
 * 
 */
UCLASS()
class LIMENWIDGETS_API ULimenTabMenu : public UWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FButtonSelectedDelegate OnButtonSelected;
	UPROPERTY(BlueprintAssignable)
	FButtonSelectedDelegate OnButtonUnselected;

	UFUNCTION(BlueprintCallable)
	TArray<UWidget*> GetMenus() const;

protected:
	UPROPERTY(EditAnywhere)
	TSubclassOf<ULimenSelectableMenuButton> MenuButton;
	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<UWidget>> MenuClasses;
	UPROPERTY(EditAnywhere)
	TArray<FText> Titles;

	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void ReleaseSlateResources(const bool bReleaseChildren) override;

	UFUNCTION()
	virtual void ButtonClicked(ULimenStandardButton* Button);

private:
	UPROPERTY()
	TArray<ULimenSelectableMenuButton*> Buttons;
	UPROPERTY()
	TArray<UWidget*> Menus;
	TSharedPtr<SWidgetSwitcher> TabSwitcher;

	UFUNCTION()
	void ButtonSelected(ULimenSelectableMenuButton* Button);
	UFUNCTION()
	void ButtonUnselected(ULimenSelectableMenuButton* Button);
};
