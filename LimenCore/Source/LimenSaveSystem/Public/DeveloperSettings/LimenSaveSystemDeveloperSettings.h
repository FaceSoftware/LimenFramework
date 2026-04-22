// Copyright FaceSoftware. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "LimenSaveSystemDeveloperSettings.generated.h"

class ULimenSaveWidget;
/**
 * 
 */
UCLASS(Config="Limen")
class LIMENSAVESYSTEM_API ULimenSaveSystemDeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	ULimenSaveSystemDeveloperSettings()
	{
		CategoryName = TEXT("Game");
		SectionName = TEXT("Limen - Save System");
	}

	UPROPERTY(EditAnywhere, Config)
	TSubclassOf<ULimenSaveWidget> SaveWidgetClass;

};
