// Copyright FaceSoftware. All Rights Reserved.


#include "UMG/LimenTabMenu.h"

#include "Blueprint/WidgetTree.h"
#include "Components/VerticalBox.h"
#include "UMG/Buttons/LimenSelectableMenuButton.h"
#include "Widgets/Layout/SSpacer.h"
#include "Widgets/Layout/SWidgetSwitcher.h"


TArray<UWidget*> ULimenTabMenu::GetMenus() const
{
	return Menus;
}

TSharedRef<SWidget> ULimenTabMenu::RebuildWidget()
{
	if (MenuClasses.Num() != Titles.Num() || MenuClasses.IsEmpty() || Titles.IsEmpty())
	{
		return SNew(SSpacer);
	}

	const TSharedRef<SHorizontalBox> ButtonsContainer = SNew(SHorizontalBox);
	for (const FText& Title : Titles)
	{
		auto* TempButton = NewObject<ULimenSelectableMenuButton>(this, MenuButton.Get());
		TempButton->SetButtonText(Title);
		TempButton->OnButtonClickedEvent.AddDynamic(this, &ULimenTabMenu::ButtonClicked);
		TempButton->OnButtonSelected.AddDynamic(this, &ULimenTabMenu::ButtonSelected);
		TempButton->OnButtonUnselected.AddDynamic(this, &ULimenTabMenu::ButtonUnselected);

		auto ButtonSlot = ButtonsContainer->AddSlot();
		ButtonSlot.AttachWidget(TempButton->TakeWidget());
		ButtonSlot.HAlign(HAlign_Fill);
		ButtonSlot.VAlign(VAlign_Fill);
		ButtonSlot.SizeParam(FStretchContent());

		Buttons.Push(TempButton);
	}
	Buttons[0]->SetButtonSelectedState(true);

	TabSwitcher = SNew(SWidgetSwitcher);
	for (auto& MenuClass : MenuClasses)
	{
		if (!MenuClass) continue;

		UWidget* TempMenu = NewObject<UWidget>(this, MenuClass.Get());

		auto MenuSlot = TabSwitcher->AddSlot();
		MenuSlot.AttachWidget(TempMenu->TakeWidget());
		MenuSlot.HAlign(HAlign_Fill);
		MenuSlot.VAlign(VAlign_Fill);

		Menus.Push(TempMenu);
	}
	TabSwitcher->SetActiveWidgetIndex(0);

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

	for (ULimenSelectableMenuButton*& Button : Buttons)
	{
		if (!Button) continue;
		CastChecked<UWidget>(Button)->ReleaseSlateResources(bReleaseChildren);
	}
	for (UWidget*& Menu : Menus)
	{
		if (!Menu) continue;
		Menu->ReleaseSlateResources(bReleaseChildren);
	}
	TabSwitcher.Reset();
}

void ULimenTabMenu::ButtonClicked(ULimenStandardButton* Button)
{
	const int32 Index = Buttons.Find(CastChecked<ULimenSelectableMenuButton>(Button));
	for (int i = 0; i < Buttons.Num(); ++i)
	{
		Buttons[i]->SetButtonSelectedState(i == Index);
	}

	TabSwitcher->SetActiveWidgetIndex(Index);
}

void ULimenTabMenu::ButtonSelected(ULimenSelectableMenuButton* Button)
{
	OnButtonSelected.Broadcast(Button);
}

void ULimenTabMenu::ButtonUnselected(ULimenSelectableMenuButton* Button)
{
	OnButtonUnselected.Broadcast(Button);
}
