// Copyright Face Software. All Rights Reserved.


#include "ItemActions/LimenHoldItemAction.h"

#include "..\..\Public\Characters\LimenPlayerCharacter.h"
#include "Items/LimenItemBase.h"
#include "Items/LimenPhysicalItem.h"
#include "Actors/LimenTool.h"
#include "Actors/LimenWeapon.h"


ULimenHoldItemAction::ULimenHoldItemAction()
{
	ActionDisplayName = FText::FromString(TEXT("Hold"));
	ActionDescription = FText::FromString(TEXT("Holds the item in hand."));
}

void ULimenHoldItemAction::SetupAction(ALimenItemBase* InItem)
{
	Super::SetupAction(InItem);
	PhysicalItem = Cast<ALimenPhysicalItem>(InItem);
	check(PhysicalItem != nullptr);
}

void ULimenHoldItemAction::ActivateAction()
{
	Super::ActivateAction();

	ALimenPlayerCharacter* OwnerCharacter = PhysicalItem->GetOwningPawn<ALimenPlayerCharacter>();
	if (OwnerCharacter == nullptr)
	{
		return;
	}

	OwnerCharacter->HoldPhysicalItem(PhysicalItem.Get());
}

bool ULimenHoldItemAction::CanPerformAction() const
{
	if (!Super::CanPerformAction())
	{
		return false;
	}

	const ALimenPlayerCharacter* OwnerCharacter = PhysicalItem->GetOwner<ALimenPlayerCharacter>();
	check(OwnerCharacter != nullptr);
	if (PhysicalItem->GetClass()->IsChildOf<ALimenTool>())
	{
		return !OwnerCharacter->IsHoldingTool();
	}

	if (PhysicalItem->GetClass()->IsChildOf<ALimenWeapon>())
	{
		return !OwnerCharacter->IsHoldingWeapon();
	}

	return true;
}
