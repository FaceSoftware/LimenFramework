// Copyright Face Software. All Rights Reserved.


#include "Settings/LimenWindowModeSetting.h"

#include "GameFramework/GameUserSettings.h"

ULimenWindowModeSetting::ULimenWindowModeSetting()
{
	DevelopmentName = TEXT("setting_windowmode");
	Category = FText::FromString(TEXT("Display"));
	DisplayName = FText::FromString(TEXT("Window Mode"));
	Description = FText::FromString(TEXT("Defines the window mode of the game."));
}

void ULimenWindowModeSetting::ApplyCurrentSetting()
{
	Super::ApplyCurrentSetting();

	GEngine->GetGameUserSettings()->SetFullscreenMode(UnFormatWindowMode(GetCurrentValue()));
	GEngine->GetGameUserSettings()->ApplySettings(false);
}

void ULimenWindowModeSetting::SetDefaults()
{
	for (int i = 0; i < EWindowMode::NumWindowModes; ++i)
	{
		PossibleSelections.Push(FormatWindowMode(static_cast<EWindowMode::Type>(i)));
	}
	DefaultSelection = FormatWindowMode(EWindowMode::Fullscreen);
}

FString ULimenWindowModeSetting::FormatWindowMode(const EWindowMode::Type InType)
{
	switch (InType)
	{
		case EWindowMode::Fullscreen: return TEXT("Fullscreen");
		case EWindowMode::WindowedFullscreen: return TEXT("Windowed Fullscreen");
		case EWindowMode::Windowed: return TEXT("Windowed");
		default: checkNoEntry();
	}
	return TEXT("Unknown");
}

EWindowMode::Type ULimenWindowModeSetting::UnFormatWindowMode(const FString& InWindowMode)
{
	if (InWindowMode == FString(TEXT("Fullscreen")))
	{
		return EWindowMode::Fullscreen;
	}
	if (InWindowMode == FString(TEXT("Windowed Fullscreen")))
	{
		return EWindowMode::WindowedFullscreen;
	}
	if (InWindowMode == FString(TEXT("Windowed")))
	{
		return EWindowMode::Windowed;
	}
	
	checkNoEntry();
	return EWindowMode::NumWindowModes;
}
