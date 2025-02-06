// Copyright © 2024 FaceSoftware. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "LimenCreditsDeveloperSettings.generated.h"


USTRUCT(BlueprintType)
struct FCreditSection
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Section;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FText> Credits;
};

/**
 * 
 */
UCLASS(Config=Limen)
class LIMENCREDITS_API ULimenCreditsDeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	static TArray<FCreditSection> GetCredits()
	{
		return GetDefault<ULimenCreditsDeveloperSettings>()->CreditsSections;
	}

	UPROPERTY(EditAnywhere, Config)
	TArray<FCreditSection> CreditsSections;

	virtual FName GetContainerName() const override { return TEXT("Project"); }
	virtual FName GetCategoryName() const override { return TEXT("Game"); }
	virtual FName GetSectionName() const override { return TEXT("Limen Credits"); }

#if WITH_EDITOR
	virtual FText GetSectionText() const { return FText::FromString(TEXT("Limen Credits")); }
#endif

protected:

private:
};
