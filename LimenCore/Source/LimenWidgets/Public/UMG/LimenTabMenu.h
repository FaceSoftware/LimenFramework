// Copyright FaceSoftware. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Buttons/LimenSelectableMenuButton.h"
#include "Components/Widget.h"
#include "LimenTabMenu.generated.h"

class ULimenStandardButton;
class ULimenSelectableMenuButton;

USTRUCT(BlueprintType)
struct FTabData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Id;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText TabTitle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UWidget> TabMenuClass;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTabButtonClicked, const FName&, ButtonId);

/**
 * 
 */
UCLASS()
class LIMENWIDGETS_API ULimenTabMenu : public UWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FTabButtonClicked OnButtonClicked;
	UPROPERTY(BlueprintAssignable)
	FButtonSelectedDelegate OnButtonSelected;
	UPROPERTY(BlueprintAssignable)
	FButtonSelectedDelegate OnButtonUnselected;

protected:
	UPROPERTY(EditAnywhere)
	TSubclassOf<ULimenSelectableMenuButton> MenuButton;
	UPROPERTY(EditAnywhere)
	TArray<FTabData> Tabs;

	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void ReleaseSlateResources(const bool bReleaseChildren) override;

	UFUNCTION()
	virtual void ButtonClicked(ULimenStandardButton* Button);

private:
	struct FTabInstanceData
	{
		FTabInstanceData(ULimenSelectableMenuButton* Button, UWidget* TabInstance, const FName& Id)
			: TabButtonInstance(Button), TabInstance(TabInstance), Id(Id) {}

		TWeakObjectPtr<ULimenSelectableMenuButton> TabButtonInstance;
		TWeakObjectPtr<UWidget> TabInstance;
		FName Id;

		bool operator==(const ULimenStandardButton* Button) const
		{
			return Button == TabButtonInstance;
		}
		bool operator==(const UWidget* Tab) const
		{
			return Tab == TabInstance;
		}
	};

	TArray<FTabInstanceData> TabInstanceData;
	TSharedPtr<SWidgetSwitcher> TabSwitcher;

	UFUNCTION()
	void ButtonSelected(ULimenSelectableMenuButton* Button);
	UFUNCTION()
	void ButtonUnselected(ULimenSelectableMenuButton* Button);
};
