// Copyright Face Software. All Rights Reserved.


#include "Archives/LimenArchive.h"

#include "UMG/LimenArchiveDisplayWidget.h"


ULimenArchive::ULimenArchive()
{
	bHasBeenRead = false;
}

bool ULimenArchive::HasBeenRead() const
{
	return bHasBeenRead;
}

void ULimenArchive::SetHasBeenRead()
{
	bHasBeenRead = true;
	OnArchiveRead.Broadcast();
}

const TArray<FText>& ULimenArchive::GetParagraphs() const
{
	return Paragraphs;
}

TSubclassOf<ULimenArchiveDisplayWidget> ULimenArchive::GetDisplayWidgetClass() const
{
	return DisplayWidgetClass.LoadSynchronous();
}
