// Copyright Face Software. All Rights Reserved.


#include "Components/LimenKeyCardReader.h"

#include "Keys/LimenKeyCard.h"


ULimenKeyCardReader::ULimenKeyCardReader(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
	MinKeyCardLevel = 0;
}

bool ULimenKeyCardReader::ReadKeyCard(ALimenKeyCard* KeyCard)
{
	return IsActive() && IsValid(KeyCard) && KeyCard->GetKeyCardLevel() >= MinKeyCardLevel;
}

int ULimenKeyCardReader::GetMinimumKeyCardLevel() const
{
	return MinKeyCardLevel;
}

void ULimenKeyCardReader::SetMinimumKeyCardLevel(const int NewMinLevel)
{
	MinKeyCardLevel = NewMinLevel;
}

