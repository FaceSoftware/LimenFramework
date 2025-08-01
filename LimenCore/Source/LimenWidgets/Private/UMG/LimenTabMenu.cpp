// Copyright FaceSoftware. All Rights Reserved.


#include "UMG/LimenTabMenu.h"

#include "Blueprint/WidgetTree.h"
#include "Components/VerticalBox.h"
#include "UMG/Buttons/LimenSelectableMenuButton.h"
#include "Widgets/Layout/SSpacer.h"
#include "Widgets/Layout/SWidgetSwitcher.h"


TSharedRef<SWidget> ULimenTabMenu::RebuildWidget()
{
	if (Tabs.IsEmpty())
	{
		return SNew(SSpacer);
	}

	TabSwitcher = SNew(SWidgetSwitcher);
	const TSharedRef<SHorizontalBox> ButtonsContainer = SNew(SHorizontalBox);
	for (const auto& [Id, TabTitle, TabMenuClass] : Tabs)
	{
		auto* TempButton = NewObject<ULimenSelectableMenuButton>(this, MenuButton.Get());
		TempButton->SetClipping(EWidgetClipping::ClipToBounds);
		TempButton->SetButtonText(TabTitle);
		TempButton->OnButtonClickedEvent.AddDynamic(this, &ULimenTabMenu::ButtonClicked);
		TempButton->OnButtonSelected.AddDynamic(this, &ULimenTabMenu::ButtonSelected);
		TempButton->OnButtonUnselected.AddDynamic(this, &ULimenTabMenu::ButtonUnselected);
		auto ButtonSlot = ButtonsContainer->AddSlot();
		ButtonSlot.AttachWidget(TempButton->TakeWidget());
		ButtonSlot.HAlign(HAlign_Fill);
		ButtonSlot.VAlign(VAlign_Fill);
		ButtonSlot.SizeParam(FStretchContent());

		UWidget* TempMenu = nullptr;
		if (TabMenuClass)
		{
			TempMenu = NewObject<UWidget>(this, TabMenuClass.Get());
			TempMenu->SetClipping(EWidgetClipping::ClipToBounds);
			auto MenuSlot = TabSwitcher->AddSlot();
			MenuSlot.AttachWidget(TempMenu->TakeWidget());
			MenuSlot.HAlign(HAlign_Fill);
			MenuSlot.VAlign(VAlign_Fill);
		}
		
		TabInstanceData.Push(FTabInstanceData(TempButton, TempMenu, Id));
	}
	
	int32 Index = 0;
	for (auto& Data : TabInstanceData)
	{
		if (Data.TabInstance.IsValid())
		{
			TabInstanceData[Index].TabButtonInstance->SetButtonSelectedState(true);
			TabSwitcher->SetActiveWidgetIndex(0);
			break;
		}
		Index++;
	}

	TSharedRef<SVerticalBox> Root = SNew(SVerticalBox)
		.Clipping(EWidgetClipping::ClipToBounds)
			+ SVerticalBox::Slot() // Buttons
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Top)
			.SizeParam(FAuto())
			[
				ButtonsContainer
			]
			+ SVerticalBox::Slot() // Menus
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			.SizeParam(FStretchContent())
			[
				TabSwitcher.ToSharedRef()
			];

	return Root;
}

void ULimenTabMenu::ReleaseSlateResources(const bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	for (auto& InstanceData : TabInstanceData)
	{
		if (InstanceData.TabButtonInstance.IsValid())
		{
			InstanceData.TabButtonInstance->ReleaseSlateResources(bReleaseChildren);
			InstanceData.TabButtonInstance.Reset();
		}
		if (InstanceData.TabInstance.IsValid())
		{
			InstanceData.TabInstance->ReleaseSlateResources(bReleaseChildren);
			InstanceData.TabInstance.Reset();
		}
	}

	TabSwitcher.Reset();
}

void ULimenTabMenu::ButtonClicked(ULimenStandardButton* Button)
{
	const int32 Index = TabInstanceData.IndexOfByKey(Button);
	if (TabInstanceData[Index].TabInstance.IsValid())
	{
		for (int i = 0; i < TabInstanceData.Num(); ++i)
		{
			TabInstanceData[i].TabButtonInstance->SetButtonSelectedState(i == Index);
		}
		if (TabSwitcher->GetActiveWidgetIndex() != Index) TabSwitcher->SetActiveWidgetIndex(Index);
	}

	OnButtonClicked.Broadcast(TabInstanceData[Index].Id);
}

void ULimenTabMenu::ButtonSelected(ULimenSelectableMenuButton* Button)
{
	OnButtonSelected.Broadcast(Button);
}

void ULimenTabMenu::ButtonUnselected(ULimenSelectableMenuButton* Button)
{
	OnButtonUnselected.Broadcast(Button);
}
