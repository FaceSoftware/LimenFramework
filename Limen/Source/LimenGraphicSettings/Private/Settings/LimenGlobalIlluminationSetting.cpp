// Copyright © FaceSoftware. All rights reserved.


#include "Settings/LimenGlobalIlluminationSetting.h"


ULimenGlobalIlluminationSetting::ULimenGlobalIlluminationSetting()
{
	DevelopmentName = TEXT("setting_gimode");
	Category = FText::FromString(TEXT("Graphics"));
	DisplayName = FText::FromString(TEXT("Global Illumination"));
	Description = FText::FromString(TEXT(""));
}

void ULimenGlobalIlluminationSetting::ApplyCurrentSetting(bool bUserRequest)
{
	Super::ApplyCurrentSetting(bUserRequest);

	if (const FString CurrentSetting = GetCurrentValue(); CurrentSetting == None)
	{
		NoneSetting.ApplyCVars();
	}
	else if (CurrentSetting == Lumen)
	{
		LumenSetting.ApplyCVars();
	}
	else if (CurrentSetting == ScreenSpace)
	{
		ScreenSpaceSetting.ApplyCVars();
	}
}

void ULimenGlobalIlluminationSetting::SetDefaults()
{
	Super::SetDefaults();

	NoneSetting.AddCVar(TEXT("r.DynamicGlobalIlluminationMethod"), 0);
	NoneSetting.AddCVar(TEXT("r.ReflectionMethod"), 0);

	LumenSetting.AddCVar(TEXT("r.DynamicGlobalIlluminationMethod"), 1);
	LumenSetting.AddCVar(TEXT("r.ReflectionMethod"), 1);

	ScreenSpaceSetting.AddCVar(TEXT("r.DynamicGlobalIlluminationMethod"), 2);
	ScreenSpaceSetting.AddCVar(TEXT("r.ReflectionMethod"), 2);

	PossibleSelections.Push(ScreenSpace);
	PossibleSelections.Push(Lumen);

	DefaultSelection = Lumen;
}
