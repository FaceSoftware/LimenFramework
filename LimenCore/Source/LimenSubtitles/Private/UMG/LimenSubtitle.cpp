// Copyright © 2024 FaceSoftware. All rights reserved.


#include "UMG/LimenSubtitle.h"

#include "TimerManager.h"
#include "LimenCore/Public/LogMacros/LimenLogMacros.h"


void ULimenSubtitle::BeginDestroy()
{
	if (GetWorld() != nullptr)
	{
		FTimerManager& TimerManager = GetWorld()->GetTimerManager();
		TimerManager.ClearTimer(CurrentCueTimerHandle);
		TimerManager.ClearTimer(NextCueTimerHandle);
	}
	
	Super::BeginDestroy();
}

void ULimenSubtitle::SetSubtitleData(const FDataTableRowHandle& InSubtitleData)
{
	SubtitleData = InSubtitleData;
	SubtitleData.DataTable->GetAllRows(TEXT("SubtitleData"), SubtitleRows);
}

void ULimenSubtitle::StartDisplayingSubtitles()
{
	if (SubtitleRows.IsEmpty())
	{
		return;
	}
	
	SubtitleIndex = 0;
	DisplayNextSubtitle();
}

void ULimenSubtitle::DisplayNextSubtitle()
{
	const FLimenSubtitleCue* CurrentCue = SubtitleRows[SubtitleIndex];
	if (CurrentCue == nullptr)
	{
		LIMEN_LOG(LogLimen, Error, this, "A nullptr row was found for \"%p\" subtitle data table", &SubtitleData.RowName);
	}
	else
	{
		ShowWidget();
		SubtitleCueSet(*CurrentCue);
	}
	
	SubtitleIndex++;

	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	TimerManager.SetTimer(CurrentCueTimerHandle, [this, CurrentCue, &TimerManager]
	{
		if (!SubtitleRows.IsValidIndex(SubtitleIndex))
		{
			OnSubtitleFinish.Broadcast(this);
			return;
		}

		HideWidget();
		
		const FLimenSubtitleCue* NextCue = SubtitleRows[SubtitleIndex];
		const float NextCueTime = NextCue->StartTime - CurrentCue->StartTime;

		TimerManager.SetTimer(NextCueTimerHandle, this, &ThisClass::DisplayNextSubtitle, NextCueTime);
	}
	, CurrentCue->EndTime - CurrentCue->StartTime, false);
}
