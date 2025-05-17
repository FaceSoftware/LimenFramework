// Copyright Face Software. All Rights Reserved.


#include "UMG/Buttons/LimenSelectableMenuButton.h"

#include "Widgets/Images/SImage.h"


ULimenSelectableMenuButton::ULimenSelectableMenuButton()
{
	bIsSelected = false;
}

void ULimenSelectableMenuButton::SetButtonSelectedState(const bool bSelected)
{
	if (bIsSelected == bSelected) return;

	bIsSelected = bSelected;
	if (bIsSelected) OnButtonSelected.Broadcast(this);
	else OnButtonUnselected.Broadcast(this);

	if (bUseIcon)
	{
		GetIcon()->SetVisibility(bIsSelected ? EVisibility::Visible : EVisibility::Hidden);
	}
}

bool ULimenSelectableMenuButton::IsButtonSelected() const
{
	return bIsSelected;
}

FReply ULimenSelectableMenuButton::OnClicked()
{
	SetButtonSelectedState(bIsSelected);
	
	return Super::OnClicked();
}

void ULimenSelectableMenuButton::OnHovered()
{
	if (!bIsSelected)
	{
		Super::OnHovered();
	}
}

void ULimenSelectableMenuButton::OnUnhovered()
{
	if (!bIsSelected)
	{
		Super::OnUnhovered();
	}
}
