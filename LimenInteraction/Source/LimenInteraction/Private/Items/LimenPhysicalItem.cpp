// Copyright Face Software. All Rights Reserved.


#include "Items/LimenPhysicalItem.h"

#include "GameFramework/Controller.h"


ALimenPhysicalItem::ALimenPhysicalItem(const FObjectInitializer& InObjectInitializer) : Super(InObjectInitializer)
{
	AttachSocketName = NAME_None;
}

const FName& ALimenPhysicalItem::GetAttachmentSocketName() const
{
	return AttachSocketName;
}
