// Copyright © 2024 FaceSoftware. All rights reserved.


#include "Subsystems/LimenSubtitlesSubsystem.h"

#include "Developer/LimenSubtitlesDeveloperSettings.h"
#include "UMG/LimenSubtitle.h"
#include "UMG/LimenSubtitleDisplay.h"


ULimenSubtitlesSubsystem::ULimenSubtitlesSubsystem()
{
}

void ULimenSubtitlesSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	auto* Settings = GetDefault<ULimenSubtitlesDeveloperSettings>();

	SubtitleDisplayWidgetClass = Settings->SubtitleDisplayWidgetClass;
	SubtitleWidgetClass = Settings->SubtitleWidgetClass;
}

void ULimenSubtitlesSubsystem::Deinitialize()
{
	if (SubtitleDisplayWidget != nullptr)
	{
		SubtitleDisplayWidget->DestroyWidget(false);
	}
	
	Super::Deinitialize();
}

void ULimenSubtitlesSubsystem::AddSubtitle(const FDataTableRowHandle& InSubtitleData)
{
	if (SubtitleDisplayWidget == nullptr || SubtitleWidgetClass.Get() == nullptr)
	{
		return;
	}
	
	ULimenSubtitle* TempSubtitleWidget = CreateWidget<ULimenSubtitle>(GetWorld(), SubtitleWidgetClass);
	TempSubtitleWidget->SetSubtitleData(InSubtitleData);
	SubtitleDisplayWidget->AddSubtitle(TempSubtitleWidget);
}

void ULimenSubtitlesSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	
	SubtitleDisplayWidget = CreateWidget<ULimenSubtitleDisplay>(&InWorld, SubtitleDisplayWidgetClass.LoadSynchronous());
	SubtitleDisplayWidget->ShowWidget();
}
