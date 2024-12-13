// Copyright Face Software. All Rights Reserved.


#include "ItemActions/LimenHolsterItemAction.h"

#include "..\..\Public\Characters\LimenPlayerCharacter.h"
#include "Items/LimenItemBase.h"
#include "Items/LimenPhysicalItem.h"
#include "Actors/LimenTool.h"
#include "Actors/LimenWeapon.h"


ULimenHolsterItemAction::ULimenHolsterItemAction()
{
	ActionDisplayName = FText::FromString(TEXT("Holster"));
	ActionDescription = FText::FromString(TEXT("Stops holding the item in hand."));
}

void ULimenHolsterItemAction::SetupAction(ALimenItemBase* InItem)
{
	Super::SetupAction(InItem);
	PhysicalItem = Cast<ALimenPhysicalItem>(InItem);
	check(PhysicalItem != nullptr);
}

void ULimenHolsterItemAction::ActivateAction()
{
	Super::ActivateAction();

	ALimenPlayerCharacter* OwnerCharacter = PhysicalItem->GetOwningPawn<ALimenPlayerCharacter>();
	if (OwnerCharacter == nullptr)
	{
		return;
	}

	OwnerCharacter->StopHoldingPhysicalItem(PhysicalItem->GetClass());
}

bool ULimenHolsterItemAction::CanPerformAction() const
{
	if (!Super::CanPerformAction())
	{
		return false;
	}

	const ALimenPlayerCharacter* OwnerCharacter = PhysicalItem->GetOwningPawn<ALimenPlayerCharacter>();
	check(OwnerCharacter != nullptr);
	if (PhysicalItem->GetClass()->IsChildOf<ALimenTool>())
	{
		return OwnerCharacter->IsHoldingTool();
	}

	if (PhysicalItem->GetClass()->IsChildOf<ALimenWeapon>())
	{
		return OwnerCharacter->IsHoldingWeapon();
	}

	return true;
}
