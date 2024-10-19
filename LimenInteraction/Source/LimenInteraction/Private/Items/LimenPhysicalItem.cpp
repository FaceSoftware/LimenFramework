// Copyright Face Software. All Rights Reserved.


#include "Items/LimenPhysicalItem.h"

#include "Components/LimenUpgradeManager.h"


ALimenPhysicalItem::ALimenPhysicalItem()
{
	PrimaryActorTick.bCanEverTick = true;
	bIsHolstered = true;
	CurrentUpgrade = 0;

	Upgrades = CreateDefaultSubobject<ULimenUpgradeManager>(TEXT("UpgradeManager"));
	check(Upgrades);
}

ULimenUpgradeManager* ALimenPhysicalItem::GetUpgradeManager_Implementation() const
{
	return Upgrades.Get();	
}

const FText& ALimenPhysicalItem::GetDisplayName_Implementation() const
{
	return DisplayName;
}

const FText& ALimenPhysicalItem::GetDescription_Implementation() const
{
	return Description;
}

UTexture2D* ALimenPhysicalItem::GetImage_Implementation() const
{
	return ItemImage.LoadSynchronous();
}

void ALimenPhysicalItem::Interact(AController* InController, APawn* InPawn)
{
	Super::Interact(InController, InPawn);

	check(InController != nullptr);
	check(InPawn != nullptr);
	
	OwningController = InController;
	OwningPawn = InPawn;
	
	SetOwner(OwningPawn.Get());
}
