// Copyright © 2024 FaceSoftware. All rights reserved.


#include "ItemActions/LimenReloadToolItemAction.h"

#include "Actors/LimenBattery.h"
#include "Actors/LimenTool.h"
#include "Components/LimenInventoryComponent.h"
#include "GameFramework/Pawn.h"


ULimenReloadToolItemAction::ULimenReloadToolItemAction()
{
	ActionDisplayName = FText::FromString(TEXT("Replace Battery"));
	ActionDescription = FText::FromString(TEXT("Reloads the tool battery."));
}

void ULimenReloadToolItemAction::SetupAction(ALimenItemBase* InItem)
{
	Super::SetupAction(InItem);
	
	Tool = Cast<ALimenTool>(InItem);
	check(Tool != nullptr);
}

void ULimenReloadToolItemAction::ActivateAction()
{
	Super::ActivateAction();
	
	ULimenInventoryComponent* InventoryComponent = Tool->GetOwningPawn()->GetComponentByClass<ULimenInventoryComponent>();
	ALimenBattery* Battery = InventoryComponent->GetItem<ALimenBattery>();
	check(Battery != nullptr);
	Tool->Recharge(Battery);
	Battery->RemoveFromGameplay();
}

bool ULimenReloadToolItemAction::CanPerformAction() const
{
	if (!Super::CanPerformAction())
	{
		return false;
	}

	ULimenInventoryComponent* InventoryComponent = Tool->GetOwningPawn()->GetComponentByClass<ULimenInventoryComponent>();
	return InventoryComponent->PeekItemInstance<ALimenBattery>() != nullptr;
}
