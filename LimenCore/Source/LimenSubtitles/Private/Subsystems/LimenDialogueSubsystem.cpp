// Copyright © 2024 FaceSoftware. All rights reserved.


#include "Subsystems/LimenDialogueSubsystem.h"

#include "Developer/LimenSubtitlesDeveloperSettings.h"
#include "DialoguePlayer/DialoguePlayerBase.h"
#include "LimenCore/Public/LogMacros/LimenLogMacros.h"
#include "UMG/LimenSubtitle.h"
#include "UMG/LimenSubtitleDisplay.h"


ULimenDialogueSubsystem::ULimenDialogueSubsystem()
{
}

void ULimenDialogueSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	auto* Settings = GetDefault<ULimenSubtitlesDeveloperSettings>();

	SubtitleDisplayWidgetClass = Settings->SubtitleDisplayWidgetClass;
	SubtitleWidgetClass = Settings->SubtitleWidgetClass;

	if (Settings->DialoguePlayerClass.Get() != nullptr)
	{
		DialoguePlayer = TStrongObjectPtr(NewObject<UDialoguePlayerBase>(this, Settings->DialoguePlayerClass));
	}
}

void ULimenDialogueSubsystem::Deinitialize()
{
	if (SubtitleDisplayWidget != nullptr)
	{
		SubtitleDisplayWidget->DestroyWidget(false);
	}
	
	Super::Deinitialize();
}

void ULimenDialogueSubsystem::AddDialogue(const UDataTable* InDialogueData)
{
	if (InDialogueData == nullptr || !InDialogueData->RowStruct->IsChildOf(FLimenDialogueCue::StaticStruct()))
	{
		LIMEN_LOG(LogLimen, Error, this, "Invalid subtitle struct.");
		return;
	}
	
	if (InDialogueData == nullptr)
	{
		return;
	}

	if (SubtitleWidgetClass != nullptr)
	{
		ULimenSubtitle* TempSubtitleWidget = CreateWidget<ULimenSubtitle>(GetWorld(), SubtitleWidgetClass);
		TempSubtitleWidget->SetSubtitleData(InDialogueData);

		if (SubtitleDisplayWidget != nullptr)
		{
			SubtitleDisplayWidget->AddSubtitle(TempSubtitleWidget);
		}
	}

	if (DialoguePlayer.IsValid())
	{
		DialoguePlayer->PlayDialogue(InDialogueData);
	}
}

void ULimenDialogueSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	if (!SubtitleDisplayWidgetClass.IsNull())
	{
		SubtitleDisplayWidget = CreateWidget<ULimenSubtitleDisplay>(&InWorld, SubtitleDisplayWidgetClass.LoadSynchronous());
		check(SubtitleDisplayWidget != nullptr)
		SubtitleDisplayWidget->ShowWidget();
	}
}
