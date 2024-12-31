// Copyright © 2024 FaceSoftware. All rights reserved.


#include "UMG/LimenSubtitleDisplay.h"

#include "TimerManager.h"
#include "Kismet/KismetArrayLibrary.h"
#include "UMG/LimenSubtitle.h"


void ULimenSubtitleDisplay::BeginDestroy()
{
	for (auto& Subtitle : ActiveSubtitles)
	{
		Subtitle->DestroyWidget(false);
	}
	ActiveSubtitles.Empty();
	
	Super::BeginDestroy();
}

void ULimenSubtitleDisplay::AddSubtitle(ULimenSubtitle* InSubtitle)
{
	check(InSubtitle != nullptr)

	InSubtitle->OnSubtitleFinish.AddLambda([this] (ULimenSubtitle* Subtitle)
	{
		RemoveFromScreen(Subtitle);	
		Subtitle->HideWidget();
		Subtitle->DestroyWidget();
		
		ActiveSubtitles.Remove(Subtitle);
	});

	AddToScreen(InSubtitle);
	InSubtitle->StartDisplayingSubtitles();

	ActiveSubtitles.Add(InSubtitle);
}

void ULimenSubtitleDisplay::SubtitleDismissed(ULimenSubtitle* Subtitle)
{
	RemoveFromScreen(Subtitle);
}

TArray<ULimenSubtitle*> ULimenSubtitleDisplay::GetSortedSubtitles() const
{
	TArray<ULimenSubtitle*> SortedSubtitles = ActiveSubtitles;
	
	const int32 ArraySize = SortedSubtitles.Num();
	for (int32 Index1 = 0, Index2 = ArraySize - 1; Index1 < ArraySize / 2; ++Index1, --Index2)
	{
		SortedSubtitles.Swap(Index1, Index2);
	}
	
	return SortedSubtitles;
}


