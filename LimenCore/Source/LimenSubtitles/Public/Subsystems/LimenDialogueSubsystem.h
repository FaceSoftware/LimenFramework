// Copyright © 2024 FaceSoftware. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Templates/SubclassOf.h"
#include "Utils/TAtomicArray.h"
#include "LimenDialogueSubsystem.generated.h"


class UDialoguePlayerBase;
class UDataTable;
struct FDataTableRowHandle;
class ULimenSubtitle;
class ULimenSubtitleDisplay;
struct FLimenDialogueCue;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDialogueEventData, const UDataTable*, DialogueData);
DECLARE_DYNAMIC_DELEGATE(FDialogueEndEvent);

/**
 * 
 */
UCLASS()
class LIMENSUBTITLES_API ULimenDialogueSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FDialogueEventData OnDialogueEnd;

	ULimenDialogueSubsystem();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable)
	bool RegisterSpeaker(const FName SpeakerId, UActorComponent* SpeakerComponent);
	UFUNCTION(BlueprintCallable)
	UActorComponent* GetSpeakerComponent(const FName SpeakerId) const;
	template<typename T>
	T* GetSpeakerComponent(const FName& SpeakerId) const
	{
		return CastChecked<T>(GetSpeakerComponent(SpeakerId), ECastCheckedType::NullAllowed);
	}

	UFUNCTION(BlueprintCallable)
	void StopDialogue(const FGuid& DialogueId);
	UFUNCTION(BlueprintCallable, DisplayName="Play Dialogue")
	UPARAM(DisplayName="Dialogue Id") FGuid BP_PlayDialogue(const UDataTable* InDialogueData, const FDialogueEndEvent OnFinished);
	FGuid PlayDialogue(const UDataTable* InDialogueData, const TFunction<void()>& OnFinished);
	FGuid PlayDialogue(const UDataTable* InDialogueData);
	UFUNCTION(BlueprintCallable)
	void UnregisterSpeaker(const FName SpeakerId);
	UFUNCTION(BlueprintCallable)
	void UnregisterAllSpeakers();

protected:
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

private:
	struct FDialogueCallbacks
	{
		FDialogueCallbacks() = default;
		~FDialogueCallbacks() = default;

		explicit FDialogueCallbacks(const UDataTable* InDialogueData)
		{
			DialogueData = InDialogueData;
		}

		void FireCallbacks()
		{
			for (TFunction<void()>& Callback : Callbacks)
			{
				if (Callback.IsSet()) Callback();
			}
		}

		bool operator==(const UDataTable* Test) const
		{
			return Test == DialogueData;
		}
		
		bool operator!=(const UDataTable* Test) const
		{
			return Test != DialogueData;
		}

		mutable const UDataTable* DialogueData;
		TArray<TFunction<void()>> Callbacks;
	};
	struct FCompleteDialogueData
	{
		TWeakObjectPtr<ULimenSubtitle> Subtitle;
		TWeakObjectPtr<UDialoguePlayerBase> DialoguePlayer;
	};
	
	TSoftClassPtr<ULimenSubtitleDisplay> SubtitleDisplayWidgetClass;
	TSubclassOf<ULimenSubtitle> SubtitleWidgetClass;

	TSubclassOf<UDialoguePlayerBase> DialoguePlayerClass;
	TArray<TStrongObjectPtr<UDialoguePlayerBase>> DialoguePlayers;
	TArray<FCompleteDialogueData> CompleteDialogueData;

	float SubtitlesDelay;
	float DialogueDelay;
	
	UPROPERTY()
	TObjectPtr<ULimenSubtitleDisplay> SubtitleDisplayWidget;

	TMap<FName, TWeakObjectPtr<UActorComponent>> SpeakersMap;
	TAtomicArray<FDialogueCallbacks> DialogueEndCallbacks;

	void DialogueFinished(UDialoguePlayerBase* DialoguePlayer);

	void DisplaySubtitles(ULimenSubtitle* Subtitle);
	void DisplayDialogue(UDialoguePlayerBase* DialoguePlayer, const UDataTable* InDialogueData);
};
