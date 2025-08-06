// Copyright © 2024 FaceSoftware. All rights reserved.


#include "Subsystems/LimenDialogueSubsystem.h"

#include "TimerManager.h"
#include "BlueprintLibraries/LimenCoreStatics.h"
#include "Developer/LimenSubtitlesDeveloperSettings.h"
#include "DialoguePlayer/DialoguePlayerBase.h"
#include "Engine/GameInstance.h"
#include "LogMacros/LimenLogMacros.h"
#include "Subsystems/LimenThreadPoolSubsystem.h"
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

bool ULimenDialogueSubsystem::RegisterSpeaker(const FName SpeakerId, UActorComponent* SpeakerComponent)
{
	if (!SpeakerComponent) return false;

	const bool bIsUnique = !SpeakersMap.Find(SpeakerId);
	check(bIsUnique)
	if (!bIsUnique) return false;
	
	SpeakersMap.Add(SpeakerId, TWeakObjectPtr(SpeakerComponent));
	LimenLog(this, FString::Printf(TEXT("Registered new speaker with id: %s"), *SpeakerId.ToString()), ELogType::Log, false);

	return true;
}

UActorComponent* ULimenDialogueSubsystem::GetSpeakerComponent(const FName SpeakerId) const
{
	if (SpeakersMap.Find(SpeakerId))
	{
		return SpeakersMap[SpeakerId].Get();
	}

	return nullptr;
}

void ULimenDialogueSubsystem::StopDialogue(const FGuid& DialogueId)
{
	const int32 DialoguePlayerIndex = DialoguePlayers.IndexOfByPredicate([this, &DialogueId] (const TStrongObjectPtr<UDialoguePlayerBase>& Test)
	{
		return Test->GetPlayerId() == DialogueId;
	});
	if (DialoguePlayerIndex == INDEX_NONE) return;

	DialoguePlayers[DialoguePlayerIndex]->StopDialogue();


	const int32 CompleteDataIndex = CompleteDialogueData.IndexOfByPredicate([this, &DialoguePlayerIndex] (const FCompleteDialogueData& Test)
	{
		return Test.DialoguePlayer.Get() == DialoguePlayers[DialoguePlayerIndex].Get();
	});
	if (DialoguePlayerIndex != INDEX_NONE)
	{
		SubtitleDisplayWidget->RemoveSubtitle(CompleteDialogueData[CompleteDataIndex].Subtitle.Get());
		CompleteDialogueData.RemoveAt(CompleteDataIndex);
	}

	DialoguePlayers.RemoveAt(DialoguePlayerIndex);
}

FGuid ULimenDialogueSubsystem::BP_PlayDialogue(const UDataTable* InDialogueData, const FDialogueEndEvent OnFinished)
{
	return PlayDialogue(InDialogueData, [OnFinished] { OnFinished.ExecuteIfBound(); });
}

FGuid ULimenDialogueSubsystem::PlayDialogue(const UDataTable* InDialogueData, const TFunction<void()>& OnFinished)
{
	const FGuid DialogueId = PlayDialogue(InDialogueData);
	
	if (DialogueId.IsValid() && OnFinished.IsSet())
	{
		FDialogueCallbacks CallbackData(InDialogueData);
		CallbackData.Callbacks.Push(OnFinished);
		DialogueEndCallbacks.Insert(CallbackData);
	}

	return DialogueId;
}

FGuid ULimenDialogueSubsystem::PlayDialogue(const UDataTable* InDialogueData)
{
	if (InDialogueData == nullptr || !InDialogueData->RowStruct->IsChildOf(FLimenDialogueCue::StaticStruct()))
	{
		LIMEN_LOG(LogLimen, Error, this, TEXT("Invalid subtitle struct."));
		return FGuid();
	}
	
	if (InDialogueData == nullptr) return FGuid();

	auto* DialoguePlayer = NewObject<UDialoguePlayerBase>(this, DialoguePlayerClass);
	DialoguePlayer->SubscribeToFinishEvent(this, &ThisClass::DialogueFinished);
	DialoguePlayers.Push(TStrongObjectPtr(DialoguePlayer));

	ULimenSubtitle* TempSubtitleWidget = SubtitleWidgetClass ? CreateWidget<ULimenSubtitle>(GetWorld(), SubtitleWidgetClass) : nullptr;
	if (SubtitleWidgetClass) TempSubtitleWidget->SetSubtitleData(InDialogueData);


	LIMEN_LOG(LogLimen, Log, this, TEXT("Instanced new dialogue player. Currently there are %d total instances."), DialoguePlayers.Num())
	
	if (FMath::IsNearlyZero(DialogueDelay))
	{
		DisplayDialogue(DialoguePlayer, InDialogueData);
	}
	else
	{
		const FTimerDelegate Delegate = FTimerDelegate::CreateUObject(this, &ThisClass::DisplayDialogue, DialoguePlayer, InDialogueData);
		FTimerHandle TempHandle;
		GetWorldRef().GetTimerManager().SetTimer(TempHandle, Delegate, SubtitlesDelay, true);
	}

	
	if (FMath::IsNearlyZero(SubtitlesDelay))
	{
		DisplaySubtitles(TempSubtitleWidget);
	}
	else
	{
		const FTimerDelegate Delegate = FTimerDelegate::CreateUObject(this, &ThisClass::DisplaySubtitles, TempSubtitleWidget);
		FTimerHandle TempHandle;
		GetWorldRef().GetTimerManager().SetTimer(TempHandle, Delegate, SubtitlesDelay, true);
	}

	{
		FCompleteDialogueData NewData;
		NewData.DialoguePlayer = DialoguePlayer;
		NewData.Subtitle = TempSubtitleWidget;
		CompleteDialogueData.Push(MoveTemp(NewData));
	}

	return DialoguePlayer->GetPlayerId();
}

void ULimenDialogueSubsystem::UnregisterSpeaker(const FName SpeakerId)
{
	SpeakersMap.Remove(SpeakerId);
}

void ULimenDialogueSubsystem::UnregisterAllSpeakers()
{
	SpeakersMap.Empty();
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

		int32 CallbackDataIndex;
		{
			const TArray<FDialogueCallbacks> CallbacksCopy = DialogueEndCallbacks.Copy();
			CallbackDataIndex = CallbacksCopy.IndexOfByKey(Data);
			if (CallbackDataIndex == INDEX_NONE) return;
		}

		FDialogueCallbacks CallbackData = DialogueEndCallbacks.PopAt(CallbackDataIndex);
		CallbackData.FireCallbacks();
		
		TWeakObjectPtr WeakThis(this);
		const TFunction<void()> RemoveCallbackData = [WeakThis]
		{
			if (WeakThis.IsValid())
			{
				WeakThis->DialogueEndCallbacks.RemoveInvalid();
			}
		};

		if (auto* ThreadPool = GetWorld()->GetGameInstance()->GetSubsystem<ULimenThreadPoolSubsystem>())
		{
			ThreadPool->AddJob(RemoveCallbackData);
		}
		else
		{
			RemoveCallbackData();
		}
	}
}

void ULimenDialogueSubsystem::DisplaySubtitles(ULimenSubtitle* Subtitle)
{
	if (SubtitleDisplayWidget != nullptr) SubtitleDisplayWidget->AddSubtitle(Subtitle);
}

void ULimenDialogueSubsystem::DisplayDialogue(UDialoguePlayerBase* DialoguePlayer, const UDataTable* InDialogueData)
{
	DialoguePlayer->PlayDialogue(InDialogueData);
}

