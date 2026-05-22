// Copyright Face Software. All Rights Reserved.


#include "ItemActions/LimenItemAction.h"


ULimenItemAction::ULimenItemAction()
{
	ActionDisplayName = FText::FromString(TEXT("DefaultActionDisplayName"));
	ActionDescription = FText::FromString(TEXT("DefaultActionDescription"));
}

void ULimenItemAction::SetupAction(ALimenItemBase* InItem)
{
	check(InItem != nullptr);
	BoundItem = InItem;
}

void ULimenItemAction::ActivateAction()
{
}

bool ULimenItemAction::CanPerformAction() const
{
	return true;
}

const FText& ULimenItemAction::GetDisplayName() const
{
	return ActionDisplayName;
}

const FText& ULimenItemAction::GetDescription() const
{
	return ActionDescription;
}

ALimenItemBase* ULimenItemAction::GetBoundItem() const
{
	return BoundItem.Get();
}
