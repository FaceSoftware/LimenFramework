// Copyright FaceSoftware. All Rights Reserved.


#include "UMG/LimenTabMenu.h"

#include "Components/VerticalBox.h"
#include "UMG/Buttons/LimenSelectableMenuButton.h"
#include "Widgets/Layout/SSpacer.h"
#include "Widgets/Layout/SWidgetSwitcher.h"


void ULimenTabMenu::AddTab(const FTabData& TabData, const int32 Index)
{
	if (!Tabs.IsValidIndex(Index))
	{
		Tabs.Push(TabData);
	}
	else
	{
		Tabs.Insert(TabData, Index);
	}

	AddTab_Internal(TabData);
}

UWidget* ULimenTabMenu::GetTabWidget(const FName TabId) const
{
	const FTabInstanceData* Data = TabInstanceData.FindByKey(TabId);
	if (!Data) return nullptr;

	return Data->TabInstance.Get();
}

TSharedRef<SWidget> ULimenTabMenu::RebuildWidget()
{
	TabSwitcher = SNew(SWidgetSwitcher);
	ButtonsContainer = SNew(SHorizontalBox);

	TabInstanceData.Empty(Tabs.Num());
	for (const auto& Data : Tabs)
	{
		AddTab_Internal(Data);
	}
	
	SetFirstActiveTab_Internal();

	TSharedRef<SVerticalBox> Root = SNew(SVerticalBox)
		.Clipping(EWidgetClipping::ClipToBounds)
			+ SVerticalBox::Slot() // Buttons
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Top)
			.SizeParam(FAuto())
			[
				ButtonsContainer.ToSharedRef()
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
	
	TabSwitcher.Reset();
	ButtonsContainer.Reset();

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
}

void ULimenTabMenu::BeginDestroy()
{
	TabInstanceData.Empty();
	Super::BeginDestroy();
}

void ULimenTabMenu::ButtonClicked(ULimenStandardButton* Button)
{
	const int32 Index = TabInstanceData.IndexOfByKey(Button);
	if (Index == INDEX_NONE) return;

	if (TabInstanceData[Index].TabInstance.IsValid() && TabInstanceData[Index].TabInstance->TakeWidget() != ActiveTabWidget)
	{
		SetActiveTab_Internal(Index);
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

void ULimenTabMenu::AddTab_Internal(const FTabData& TabData)
{
	if (!TabData.TabButtonClass) { return; }

	auto* TempButton = NewObject<ULimenSelectableMenuButton>(this, TabData.TabButtonClass.Get());
	TempButton->SetClipping(EWidgetClipping::ClipToBounds);
	TempButton->SetButtonText(TabData.TabTitle);
	TempButton->OnButtonClickedEvent.AddDynamic(this, &ULimenTabMenu::ButtonClicked);
	TempButton->OnButtonSelected.AddDynamic(this, &ULimenTabMenu::ButtonSelected);
	TempButton->OnButtonUnselected.AddDynamic(this, &ULimenTabMenu::ButtonUnselected);

	auto ButtonSlot = ButtonsContainer->AddSlot();
	ButtonSlot.AttachWidget(TempButton->TakeWidget());
	ButtonSlot.HAlign(HAlign_Fill);
	ButtonSlot.VAlign(VAlign_Fill);
	ButtonSlot.SizeParam(FStretchContent());
	ButtonsContainer->Invalidate(EInvalidateWidgetReason::ChildOrder);

	UWidget* TempMenu = nullptr;
	if (TabData.TabMenuClass)
	{
		TempMenu = NewObject<UWidget>(this, TabData.TabMenuClass.Get());
		TempMenu->SetClipping(EWidgetClipping::ClipToBounds);

		auto MenuSlot = TabSwitcher->AddSlot(0);
		MenuSlot.AttachWidget(TempMenu->TakeWidget());
		MenuSlot.HAlign(HAlign_Fill);
		MenuSlot.VAlign(VAlign_Fill);
		TabSwitcher->Invalidate(EInvalidateWidgetReason::ChildOrder);
	}
	
	TabInstanceData.Push(FTabInstanceData(TempButton, TempMenu, TabData.Id));

	if (!ActiveTabWidget.IsValid()) { SetActiveTab_Internal(0); }
}

bool ULimenTabMenu::SetActiveTab_Internal(const int32 Index)
{
	if (!TabInstanceData.IsValidIndex(Index)) { return false; }
	if (!TabInstanceData[Index].TabInstance.IsValid()) { return false; }

	ActiveTabWidget = TabInstanceData[Index].TabInstance->TakeWidget();	
	TabSwitcher->SetActiveWidget(ActiveTabWidget.Pin().ToSharedRef()); 

	for (int i = 0; i < TabInstanceData.Num(); ++i)
	{
		TabInstanceData[i].TabButtonInstance->SetButtonSelectedState(i == Index);
	}

	return true;
}

void ULimenTabMenu::SetFirstActiveTab_Internal()
{
	for (int i = 0; i < TabInstanceData.Num(); ++i)
	{
		if (SetActiveTab_Internal(i)) { return; }
	}
}
