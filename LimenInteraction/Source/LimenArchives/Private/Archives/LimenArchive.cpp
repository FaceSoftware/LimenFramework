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

const FText& ULimenArchive::GetFullText() const
{
	if (!FullArchiveText.IsSet())
	{
		FString Result = TEXT("");
		for (const FText& Paragraph : Paragraphs)
		{
			Result += Paragraph.ToString() + "\n\n";
		}

		FullArchiveText = FText::FromString(Result);
	}

	return FullArchiveText.GetValue();
}

UTexture* ULimenArchive::GetImage() const
{
	return ImageTexture.LoadSynchronous();
}

TSubclassOf<ULimenArchiveDisplayWidget> ULimenArchive::GetDisplayWidgetClass() const
{
	return DisplayWidgetClass.LoadSynchronous();
}
