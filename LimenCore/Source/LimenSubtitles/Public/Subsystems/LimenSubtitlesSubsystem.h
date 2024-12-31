// Copyright © 2024 FaceSoftware. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Templates/SubclassOf.h"
#include "UObject/StrongObjectPtr.h"
#include "LimenSubtitlesSubsystem.generated.h"


struct FDataTableRowHandle;
class ULimenSubtitle;
class ULimenSubtitleDisplay;
struct FLimenSubtitleCue;
/**
 * 
 */
UCLASS()
class LIMENSUBTITLES_API ULimenSubtitlesSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	ULimenSubtitlesSubsystem();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable)
	virtual void AddSubtitle(const FDataTableRowHandle& InSubtitleData);

protected:
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

private:
	TSoftClassPtr<ULimenSubtitleDisplay> SubtitleDisplayWidgetClass;
	TSubclassOf<ULimenSubtitle> SubtitleWidgetClass;
	
	UPROPERTY()
	TObjectPtr<ULimenSubtitleDisplay> SubtitleDisplayWidget;

};
