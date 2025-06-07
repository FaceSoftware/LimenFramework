// Copyright © 2024 FaceSoftware. All rights reserved.


#include "Subsystems/LimenDialogueSubsystem.h"

#include "TimerManager.h"
#include "Developer/LimenSubtitlesDeveloperSettings.h"
#include "DialoguePlayer/DialoguePlayerBase.h"
#include "LimenCore/Public/LogMacros/LimenLogMacros.h"
#include "UMG/LimenSubtitle.h"
#include "UMG/LimenSubtitleDisplay.h"


ULimenDialogueSubsystem::ULimenDialogueSubsystem() : Super(), SubtitlesDelay(0), DialogueDelay(0)
{
}

void ULimenDialogueSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	auto* Settings = GetDefault<ULimenSubtitlesDeveloperSettings>();

	SubtitleDisplayWidgetClass = Settings->SubtitleDisplayWidgetClass;
	SubtitleWidgetClass = Settings->SubtitleWidgetClass;
	DialoguePlayerClass = Settings->DialoguePlayerClass;
	SubtitlesDelay = Settings->SubtitlesDelay;
	SubtitlesDelay = Settings->DialogueDelay;
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

void ULimenDialogueSubsystem::RegisterSpeaker(const FName SpeakerId, const TScriptInterface<ILimenDialogueSpeaker>& Speaker)
{
#if WITH_EDITOR

	if (!Speaker.GetObject())
	{
		return;
	}

	const TScriptInterface<ILimenDialogueSpeaker>* Value = SpeakersMap.Find(SpeakerId);
	if (!ensureAlwaysMsgf(!Value, TEXT("Duplicated speaker id!")))
	{
		return;
	}

#endif // WITH_EDITOR
	
	SpeakersMap.Add(SpeakerId, Speaker);
}

TScriptInterface<ILimenDialogueSpeaker> ULimenDialogueSubsystem::GetSpeaker(const FName SpeakerId) const
{
	if (SpeakersMap.Find(SpeakerId))
	{
		return SpeakersMap[SpeakerId];
	}

	return nullptr;
}

void ULimenDialogueSubsystem::PlayDialogue(const UDataTable* InDialogueData, FDialogueEndEvent OnFinished)
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

	DialogueEndCallbacks.Add(TWeakObjectPtr(InDialogueData), OnFinished);

	if (FMath::IsNearlyZero(DialogueDelay))
	{
		DisplayDialogue(InDialogueData);
	}
	else
	{
		const FTimerDelegate Delegate = FTimerDelegate::CreateUObject(this, &ThisClass::DisplayDialogue, InDialogueData);
		FTimerHandle TempHandle;
		GetWorldRef().GetTimerManager().SetTimer(TempHandle, Delegate, SubtitlesDelay, true);
	}

	
	if (FMath::IsNearlyZero(SubtitlesDelay))
	{
		DisplaySubtitles(InDialogueData);
	}
	else
	{
		const FTimerDelegate Delegate = FTimerDelegate::CreateUObject(this, &ThisClass::DisplaySubtitles, InDialogueData);
		FTimerHandle TempHandle;
		GetWorldRef().GetTimerManager().SetTimer(TempHandle, Delegate, SubtitlesDelay, true);
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
		const UDataTable* Data = DialoguePlayers[Index]->GetDialogueData();
		DialoguePlayers.RemoveAt(Index);

		OnDialogueEnd.Broadcast(Data);
		const FDialogueEndEvent& Callback = DialogueEndCallbacks.FindChecked(TWeakObjectPtr(Data));
		Callback.ExecuteIfBound();
	}
}

void ULimenDialogueSubsystem::DisplaySubtitles(const UDataTable* InDialogueData)
{
	if (SubtitleWidgetClass != nullptr)
	{
		ULimenSubtitle* TempSubtitleWidget = CreateWidget<ULimenSubtitle>(GetWorld(), SubtitleWidgetClass);
		TempSubtitleWidget->SetSubtitleData(InDialogueData);

		if (SubtitleDisplayWidget != nullptr)
		{
			SubtitleDisplayWidget->AddSubtitle(TempSubtitleWidget);
		}
	}
}

void ULimenDialogueSubsystem::DisplayDialogue(const UDataTable* InDialogueData)
{
	if (DialoguePlayerClass.Get() != nullptr)
	{
		const TStrongObjectPtr DialoguePlayer = TStrongObjectPtr(NewObject<UDialoguePlayerBase>(this, DialoguePlayerClass));
		DialoguePlayer->SubscribeToFinishEvent(this, &ThisClass::DialogueFinished);
		DialoguePlayer->PlayDialogue(InDialogueData);

		DialoguePlayers.Push(DialoguePlayer);
	}

	LIMEN_LOG(LogLimen, Log, this, "Instanced new dialogue player. Currently there are %d active instances.", DialoguePlayers.Num())
}

