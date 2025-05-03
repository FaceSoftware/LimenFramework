// Copyright Face Software. All Rights Reserved.


#include "Items/LimenPhysicalItem.h"

#include "Components/LimenUpgradeManager.h"
#include "GameFramework/Controller.h"


ALimenPhysicalItem::ALimenPhysicalItem(const FObjectInitializer& InObjectInitializer) : Super(InObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	bIsHolstered = true;
	CurrentUpgrade = 0;
	AttachSocket = NAME_None;

	Upgrades = CreateDefaultSubobject<ULimenUpgradeManager>(TEXT("UpgradeManager"));
	check(Upgrades);
}

const FName& ALimenPhysicalItem::GetAttachmentSocket() const
{
	return AttachSocket;
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

UTexture* ALimenPhysicalItem::GetItemImage_Implementation() const
{
	return ItemImage.Get();
}

void ALimenPhysicalItem::Upgrade_Implementation(int32 NewLevel, ULimenUpgradeDataAsset* Upgrade)
{
}
