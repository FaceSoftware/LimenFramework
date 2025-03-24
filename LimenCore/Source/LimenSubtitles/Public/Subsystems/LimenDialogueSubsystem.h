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
/**
 * 
 */
UCLASS()
class LIMENSUBTITLES_API ULimenDialogueSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	ULimenDialogueSubsystem();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable)
	virtual void AddDialogue(const UDataTable* InDialogueData);

protected:
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

private:
	TSoftClassPtr<ULimenSubtitleDisplay> SubtitleDisplayWidgetClass;
	TSubclassOf<ULimenSubtitle> SubtitleWidgetClass;

	TSubclassOf<UDialoguePlayerBase> DialoguePlayerClass;
	TArray<TStrongObjectPtr<UDialoguePlayerBase>> DialoguePlayers;
	
	UPROPERTY()
	TObjectPtr<ULimenSubtitleDisplay> SubtitleDisplayWidget;

	virtual void DialogueFinished(UDialoguePlayerBase* DialoguePlayer);

};
