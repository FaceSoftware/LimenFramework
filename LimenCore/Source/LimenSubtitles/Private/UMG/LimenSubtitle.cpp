// Copyright © 2024 FaceSoftware. All rights reserved.


#include "UMG/LimenSubtitle.h"

#include "TimerManager.h"
#include "LimenCore/Public/LogMacros/LimenLogMacros.h"


ULimenSubtitle::ULimenSubtitle(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer), SubtitleIndex(0)
{
	SetWidgetLevel(21);
}

void ULimenSubtitle::NativeConstruct()
{
	Super::NativeConstruct();

	HideWidget();
}

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

void ULimenSubtitle::SetSubtitleData(const UDataTable* InSubtitleData)
{
	if (InSubtitleData == nullptr)
	{
		return;
	}
	
	SubtitleData = TStrongObjectPtr(InSubtitleData);
	SubtitleData->GetAllRows(TEXT("SubtitleData"), SubtitleRows);
}

void ULimenSubtitle::StartDisplayingSubtitles()
{
	if (SubtitleRows.IsEmpty())
	{
		return;
	}
	
	SubtitleIndex = 0;
	
	if (const FLimenSubtitleCue* CurrentCue = SubtitleRows[SubtitleIndex]; !FMath::IsNearlyZero(CurrentCue->StartTime))
	{		
		FTimerManager& TimerManager = GetWorld()->GetTimerManager();
		TimerManager.SetTimer(CurrentCueTimerHandle, this, &ThisClass::ShowCurrentSubtitle, CurrentCue->StartTime, false);
	}
	else
	{
		ShowCurrentSubtitle();
	}
}

void ULimenSubtitle::ShowCurrentSubtitle()
{
	const FLimenSubtitleCue* CurrentCue = SubtitleRows[SubtitleIndex];
	if (CurrentCue == nullptr)
	{
		const FName RowName = SubtitleData->GetRowNames()[SubtitleIndex];
		LIMEN_LOG(LogLimen, Error, this, "Unable to get subtitle data table row with name \"%p\"", &RowName);
	}
	else
	{
		ShowWidget();
		SubtitleCueSet(*CurrentCue);
	}
	
	const float CueDuration = CurrentCue->EndTime - CurrentCue->StartTime;
	
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	TimerManager.SetTimer(CurrentCueTimerHandle, this, &ThisClass::HideCurrentSubtitle, CueDuration, false);
}

void ULimenSubtitle::HideCurrentSubtitle()
{
	HideWidget();
	
	if (!SubtitleRows.IsValidIndex(SubtitleIndex + 1))
	{
		OnSubtitleFinish.Broadcast(this);
		return;
	}
	
	const FLimenSubtitleCue* CurrentCue = SubtitleRows[SubtitleIndex];
	const FLimenSubtitleCue* NextCue = SubtitleRows[++SubtitleIndex];

	const float SecondsUntilNextCue = NextCue->StartTime - CurrentCue->EndTime;
	
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	TimerManager.SetTimer(CurrentCueTimerHandle, this, &ThisClass::ShowCurrentSubtitle, SecondsUntilNextCue, false);
}
