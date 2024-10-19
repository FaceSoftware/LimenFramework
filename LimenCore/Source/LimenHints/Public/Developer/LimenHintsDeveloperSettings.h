// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "LimenHintsDeveloperSettings.generated.h"


class ULimenHintWidget;
/**
 * 
 */
UCLASS(Config="Limen")
class LIMENHINTS_API ULimenHintsDeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Config, Category="Classes")
	TSoftClassPtr<ULimenHintWidget> DefaultHintWidget;
	
	virtual FName GetContainerName() const override;
	virtual FName GetCategoryName() const override;
	virtual FName GetSectionName() const override;

#if WITH_EDITOR
	virtual FText GetSectionText() const override;
	virtual FText GetSectionDescription() const override;
#endif
	
};
