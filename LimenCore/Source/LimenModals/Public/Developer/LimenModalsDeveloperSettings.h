// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "LimenModalsDeveloperSettings.generated.h"

class ULimenGenericModalWidget;
/**
 * 
 */
UCLASS(Config="Limen")
class LIMENMODALS_API ULimenModalsDeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Config, Category="Limen|Modals")
	TSoftClassPtr<ULimenGenericModalWidget> ConfirmationModalClass;
	
	UPROPERTY(EditAnywhere, Config, Category="Limen|Modals")
	TSoftClassPtr<ULimenGenericModalWidget> ConsentModalClass;

	ULimenModalsDeveloperSettings()
	{
		SectionName = TEXT("Limen - Modals System");
	}
};
