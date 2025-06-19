// Copyright © 2024 FaceSoftware. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Templates/SubclassOf.h"
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
	void RegisterSpeaker(const FName SpeakerId, UActorComponent* SpeakerComponent);
	UFUNCTION(BlueprintCallable)
	UActorComponent* GetSpeakerComponent(const FName SpeakerId) const;
	template<typename T>
	T* GetSpeakerComponent(const FName& SpeakerId) const
	{
		return CastChecked<T>(GetSpeakerComponent(SpeakerId), ECastCheckedType::NullAllowed);
	}

	UFUNCTION(BlueprintCallable)
	virtual void PlayDialogue(const UDataTable* InDialogueData, FDialogueEndEvent OnFinished);
	UFUNCTION(BlueprintCallable)
	void UnregisterSpeaker(const FName SpeakerId);
	UFUNCTION(BlueprintCallable)
	void UnregisterAllSpeakers();

protected:
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

private:
	TSoftClassPtr<ULimenSubtitleDisplay> SubtitleDisplayWidgetClass;
	TSubclassOf<ULimenSubtitle> SubtitleWidgetClass;

	TSubclassOf<UDialoguePlayerBase> DialoguePlayerClass;
	TArray<TStrongObjectPtr<UDialoguePlayerBase>> DialoguePlayers;

	float SubtitlesDelay;
	float DialogueDelay;
	
	UPROPERTY()
	TObjectPtr<ULimenSubtitleDisplay> SubtitleDisplayWidget;

	TMap<FName, TWeakObjectPtr<UActorComponent>> SpeakersMap;
	TMap<TWeakObjectPtr<const UDataTable>, FDialogueEndEvent> DialogueEndCallbacks;

	void DialogueFinished(UDialoguePlayerBase* DialoguePlayer);

	void DisplaySubtitles(const UDataTable* InDialogueData);
	void DisplayDialogue(const UDataTable* InDialogueData);
};
