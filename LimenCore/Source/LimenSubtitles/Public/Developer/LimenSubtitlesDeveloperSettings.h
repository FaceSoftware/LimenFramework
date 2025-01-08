// Copyright © 2024 FaceSoftware. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "Templates/SubclassOf.h"
#include "LimenSubtitlesDeveloperSettings.generated.h"

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
	UPROPERTY(EditAnywhere, Config)
	TSubclassOf<ULimenSubtitleDisplay> SubtitleDisplayWidgetClass;
	UPROPERTY(EditAnywhere, Config)
	TSubclassOf<ULimenSubtitle> SubtitleWidgetClass;
	
	ULimenSubtitlesDeveloperSettings()
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
