// Copyright Face Software. All Rights Reserved.


#include "Items/LimenPhysicalItem.h"

#include "GameFramework/Controller.h"


ALimenPhysicalItem::ALimenPhysicalItem(const FObjectInitializer& InObjectInitializer) : Super(InObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	bIsHolstered = true;
	CurrentUpgrade = 0;
	AttachSocket = NAME_None;
}

const FName& ALimenPhysicalItem::GetAttachmentSocket() const
{
	return AttachSocket;
}
