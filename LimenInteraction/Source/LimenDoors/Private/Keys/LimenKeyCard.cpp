// Copyright Face Software. All Rights Reserved.


#include "Keys/LimenKeyCard.h"

ALimenKeyCard::ALimenKeyCard(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	KeyCardLevel = 0;
}

int ALimenKeyCard::GetKeyCardLevel() const
{
	return KeyCardLevel;
}
