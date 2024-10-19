// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "Subsystems/LimenGlobalRandomStreamSubsystem.h"
#include "LimenLoadingScreenSettings.generated.h"

class ULimenLoadingScreenParameters;
/**
 * 
 */
UCLASS(Config="Limen")
class LIMENLEVELTRANSITIONS_API ULimenLoadingScreenSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:	
	UPROPERTY(EditAnywhere, Config)
	TMap<TSoftObjectPtr<UWorld>, TSoftObjectPtr<ULimenLoadingScreenParameters>> LoadingScreens;

	UPROPERTY(EditAnywhere, Config)
	TArray<FText> Hints;

	ULimenLoadingScreenSettings()
	{
		CategoryName = TEXT("Game");
		SectionName = TEXT("Limen - Loading Screens");
	}
	
	UFUNCTION(BlueprintCallable, Category="Limen|Level Transitions|Hints", meta=(ExpandBoolAsExecs="ReturnValue"))
	static UPARAM(DisplayName="Success") bool GetRandomHint(FText& OutHint)
	{
		const auto* HintsSettings = GetDefault<ULimenLoadingScreenSettings>();
		if (HintsSettings->Hints.IsEmpty())
		{
			return false;
		}
		
		if (!OutHint.IsEmpty())
		{
			OutHint = FText::FromString(TEXT(""));
		}
		
		auto* RandomInstanceSubsystem = ULimenGlobalRandomStreamSubsystem::Get();
		if (RandomInstanceSubsystem == nullptr)
		{
			return false;
		}
		const uint32 Max = HintsSettings->Hints.Num() - 1;
		const uint32 RandomIndex = RandomInstanceSubsystem->RandomIntRange(Max, 0);
		OutHint = HintsSettings->Hints[RandomIndex];
		return true;
	}

#if WITH_EDITOR
	virtual FText GetSectionText() const override
	{
		return FText::FromName(SectionName);
	}
	
	virtual FText GetSectionDescription() const override
	{
		return FText::FromString(TEXT("Configure loading screens when loading specific maps"));
	}
#endif
};
