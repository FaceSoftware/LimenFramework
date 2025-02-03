// Copyright Face Software. All Rights Reserved.


#include "Items/LimenPhysicalItem.h"

#include "Components/LimenUpgradeManager.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"


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

UTexture2D* ALimenPhysicalItem::GetItemImage_Implementation() const
{
	return ItemImage.LoadSynchronous();
}

void ALimenPhysicalItem::Upgrade_Implementation(int32 NewLevel, ULimenUpgradeDataAsset* Upgrade)
{
}

void ALimenPhysicalItem::SetOwner(AActor* NewOwner)
{
	Super::SetOwner(NewOwner);
	
	OwningController = Cast<AController>(NewOwner);
	OwningPawn = Cast<APawn>(NewOwner);
}

void ALimenPhysicalItem::Interact(AController* InController, APawn* InPawn)
{
	Super::Interact(InController, InPawn);

	check(InController != nullptr);
	check(InPawn != nullptr);

	SetOwner(OwningPawn.Get());
}
