// Copyright © 2024 FaceSoftware. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "Templates/SubclassOf.h"
#include "UMG/LimenSubtitle.h"
#include "UMG/LimenSubtitleDisplay.h"
#include "LimenSubtitlesDeveloperSettings.generated.h"

class UDialoguePlayerBase;
class ULimenSubtitleDisplay;
class ULimenSubtitle;
/**
 * 
 */
UCLASS(Config=Limen)
class LIMENSUBTITLES_API ULimenSubtitlesDeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Config, meta=(ClampMin="0"))
	float SubtitlesDelay;
	UPROPERTY(EditAnywhere, Config, meta=(ClampMin="0"))
	float DialogueDelay;
	UPROPERTY(EditAnywhere, Config)
	TSubclassOf<ULimenSubtitleDisplay> SubtitleDisplayWidgetClass;
	UPROPERTY(EditAnywhere, Config)
	TSubclassOf<ULimenSubtitle> SubtitleWidgetClass;
	UPROPERTY(EditAnywhere, Config)
	TSubclassOf<UDialoguePlayerBase> DialoguePlayerClass;
	
	ULimenSubtitlesDeveloperSettings(): SubtitlesDelay(0), DialogueDelay(0)
	{
		CategoryName = TEXT("Game");
		SectionName = TEXT("Limen Subtitles");
	}

	virtual FName GetContainerName() const override
	{
		return TEXT("Project");
	}

#if WITH_EDITOR

	virtual FText GetSectionText() const override
	{
		return FText::FromName(SectionName);
	}
	
#endif
};
