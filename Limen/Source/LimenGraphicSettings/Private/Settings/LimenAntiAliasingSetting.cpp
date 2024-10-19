// Copyright Face Software. All Rights Reserved.


#include "Settings/LimenAntiAliasingSetting.h"


ULimenAntiAliasingSetting::ULimenAntiAliasingSetting()
{
	DevelopmentName = TEXT("setting_antialiasin");
	Category = FText::FromString(TEXT("Graphics"));
	DisplayName = FText::FromString(TEXT("Anti Aliasing Method"));
	Description = FText::FromString(TEXT("Anti-aliasing reduces visual artifacts such as jagged edges by smoothing the transitions between contrasting pixels."));
}

void ULimenAntiAliasingSetting::ApplyCurrentSetting()
{
	Super::ApplyCurrentSetting();

	IConsoleVariable* AntiAliasingAConsoleVariable = IConsoleManager::Get().FindConsoleVariable(TEXT("r.AntiAliasingMethod"));
	check(AntiAliasingAConsoleVariable != nullptr);
	AntiAliasingAConsoleVariable->Set(UnFormatAntiAliasingMode(GetCurrentValue()));
}

void ULimenAntiAliasingSetting::SetDefaults()
{
	Super::SetDefaults();

	PossibleSelections.Reserve(5);
	PossibleSelections.Push(FXAA);
	PossibleSelections.Push(TAA);
	PossibleSelections.Push(MSAA);
	PossibleSelections.Push(TSR);

	DefaultSelection = TSR;
}

uint8 ULimenAntiAliasingSetting::UnFormatAntiAliasingMode(const FString& Mode) const
{
	if (Mode.Compare(FXAA) == 0)
	{
		return 1;
	}
	if (Mode.Compare(TAA) == 0)
	{
		return 2;
	}
	if (Mode.Compare(MSAA) == 0)
	{
		return 3;
	}
	if (Mode.Compare(TSR) == 0)
	{
		return 4;
	}

	checkNoEntry();
	return {};
}

FString ULimenAntiAliasingSetting::FormatAntiAliasingMode(const uint8 Mode) const
{
	switch (Mode)
	{
	case 1:
		return FXAA;

	case 2:
		return TAA;
		
	case 3:
		return MSAA;
		
	case 4:
		return TSR;
		
	default:
		break;
	}

	checkNoEntry();
	return {};
}
