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
	DialoguePlayerClass = Settings->DialoguePlayerClass;
}

void ULimenDialogueSubsystem::Deinitialize()
{
	for (auto& Player : DialoguePlayers)
	{
		Player.Reset();
	}
	
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

	if (DialoguePlayerClass.Get() != nullptr)
	{
		const TStrongObjectPtr DialoguePlayer = TStrongObjectPtr(NewObject<UDialoguePlayerBase>(this, DialoguePlayerClass));
		DialoguePlayer->PlayDialogue(InDialogueData);

		DialoguePlayers.Push(DialoguePlayer);
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

void ULimenDialogueSubsystem::DialogueFinished(UDialoguePlayerBase* DialoguePlayer)
{
	const int32 Index = DialoguePlayers.IndexOfByPredicate([&DialoguePlayer] (const TStrongObjectPtr<UDialoguePlayerBase>& Test)
	{
		return Test.Get() == DialoguePlayer;
	});

	if (DialoguePlayers.IsValidIndex(Index))
	{
		DialoguePlayers.RemoveAt(Index);
	}
}
