// Copyright Face Software. All Rights Reserved.


#include "Settings/LimenOverallQualitySetting.h"

#include "GameFramework/GameUserSettings.h"

ULimenOverallQualitySetting::ULimenOverallQualitySetting()
{
	DevelopmentName = TEXT("setting_overallquality");
	Category = FText::FromString(TEXT("Graphics"));
	DisplayName = FText::FromString(TEXT("Overall Quality"));
	Description = FText::FromString(TEXT("Overall quality"));
}

void ULimenOverallQualitySetting::ApplyCurrentSetting(const bool bUserRequest)
{
	Super::ApplyCurrentSetting();

	GEngine->GetGameUserSettings()->SetOverallScalabilityLevel(UnFormatReflectionSetting(GetCurrentValue()));
	GEngine->GetGameUserSettings()->ApplySettings(false);
}

void ULimenOverallQualitySetting::SetDefaults()
{
	PossibleSelections.Push(High);
	PossibleSelections.Push(Ultra);
	PossibleSelections.Push(Cinematic);
	DefaultSelection = High;
}

FString ULimenOverallQualitySetting::FormatReflectionSetting(const int32 Method)
{
	switch (Method)
	{
	case 2:
		return High;

	case 3:
		return Ultra;

	case 4:
		return Cinematic;
		
	default:
		checkNoEntry();
	}

	return {};
}

int32 ULimenOverallQualitySetting::UnFormatReflectionSetting(const FString& Selection)
{
	if (Selection.Compare(High) == 0)
	{
		return 2;
	}
	if (Selection.Compare(Ultra) == 0)
	{
		return 3;
	}
	if (Selection.Compare(Cinematic) == 0)
	{
		return 4;
	}
	
	checkNoEntry();
	return -1;
}
