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
	TSubclassOf<ULimenSelectableMenuButton> TabButtonClass;
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
	
	virtual void ReleaseSlateResources(const bool bReleaseChildren) override;
	virtual void BeginDestroy() override;

	UFUNCTION(BlueprintCallable)
	UWidget* AddTab(const FTabData& TabData, int32 Index = -1);
	template<typename T>
	T* AddTab(const FTabData& TabData, const int32 Index = -1)
	{
		static_assert(TIsDerivedFrom<T, UWidget>::IsDerived);
		return Cast<T>(AddTab(TabData, Index));
	}
	UFUNCTION(BlueprintCallable)
	UWidget* GetTabWidget(const FName TabId) const;
	template<typename T>
	T* GetTabWidget(const FName TabId) const
	{
		static_assert(TIsDerivedFrom<T, UWidget>::IsDerived);
		return Cast<T>(GetTabWidget(TabId));
	}
	
	void ClearTabs();

protected:
	UPROPERTY(EditAnywhere)
	TArray<FTabData> Tabs;

	virtual TSharedRef<SWidget> RebuildWidget() override;

	UFUNCTION()
	virtual void ButtonClicked(ULimenStandardButton* Button);

private:
	struct FTabInstanceData
	{
		FTabInstanceData(ULimenSelectableMenuButton* Button, UWidget* TabInstance, const FName& Id)
			: TabButtonInstance(Button), TabInstance(TabInstance), Id(Id) {}

		TStrongObjectPtr<ULimenSelectableMenuButton> TabButtonInstance;
		TStrongObjectPtr<UWidget> TabInstance;
		FName Id;

		bool operator==(const ULimenStandardButton* Button) const
		{
			return Button == TabButtonInstance.Get();
		}
		bool operator==(const UWidget* Tab) const
		{
			return Tab == TabInstance.Get();
		}
		bool operator==(const FName& InId) const
		{
			return InId == Id;
		}
		operator bool() const
		{
			return TabButtonInstance.IsValid();
		}
	};

	TArray<FTabInstanceData> TabInstanceData;
	
	TSharedPtr<SWidgetSwitcher> TabSwitcher;
	TSharedPtr<SHorizontalBox> ButtonsContainer;
	
	TWeakPtr<SWidget> ActiveTabWidget;

	UFUNCTION()
	void ButtonSelected(ULimenSelectableMenuButton* Button);
	UFUNCTION()
	void ButtonUnselected(ULimenSelectableMenuButton* Button);

	UWidget* AddTab_Internal(const FTabData& TabData);
	bool SetActiveTab_Internal(const int32 Index);
	void SetFirstActiveTab_Internal();
};
