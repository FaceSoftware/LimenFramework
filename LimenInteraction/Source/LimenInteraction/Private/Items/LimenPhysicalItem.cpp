// Copyright Face Software. All Rights Reserved.


#include "Items/LimenPhysicalItem.h"

#include "GameFramework/Controller.h"


ALimenPhysicalItem::ALimenPhysicalItem(const FObjectInitializer& InObjectInitializer) : Super(InObjectInitializer)
{
	AttachSocketName = NAME_None;
}

void ALimenPhysicalItem::SetTagState(const FGameplayTag& InTag, bool bEnable)
{
	if (!CanSetTagState(InTag, bEnable)) { return; }
	bEnable ? ItemStateTags.AddTag(InTag) : ItemStateTags.RemoveTag(InTag);
	TagStateChanged(InTag, bEnable);
}

const FName& ALimenPhysicalItem::GetAttachmentSocketName() const
{
	return AttachSocketName;
}

void ALimenPhysicalItem::TagStateChanged(const FGameplayTag& Tag, bool bState)
{
}

bool ALimenPhysicalItem::CanSetTagState(const FGameplayTag& InTag, const bool bState)
{
	return ItemStateTags.HasTag(InTag) != bState;
}

FGameplayTagContainer& ALimenPhysicalItem::GetItemStateTags()
{
	return ItemStateTags;
}
