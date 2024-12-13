// Copyright Face Software. All Rights Reserved.


#include "Settings/LimenResolutionSetting.h"

#include "BlueprintLibraries/LimenString.h"
#include "GameFramework/GameUserSettings.h"
#include "Settings/LimenWindowModeSetting.h"
#include "Subsystems/LimenModularSettingsSubsystem.h"


ULimenResolutionSetting::ULimenResolutionSetting()
{
	DevelopmentName = TEXT("setting_resolution");
	Category = FText::FromString(TEXT("Display"));
	DisplayName = FText::FromString(TEXT("Resolution"));
	Description = FText::FromString(TEXT("Defines the display resolution, higher is better."));
}

void ULimenResolutionSetting::SubsystemInitialized(ULimenModularSettingsSubsystem* ModularSettingsSubsystem)
{
	Super::SubsystemInitialized(ModularSettingsSubsystem);

	ULimenSetting* Setting = ModularSettingsSubsystem->GetItem<ULimenSetting>(ULimenWindowModeSetting::StaticClass());
	Setting->OnSettingUpdated.AddUniqueDynamic(this, &ThisClass::OnWindowModeUpdated);
}

bool ULimenResolutionSetting::CanEdit() const
{
	return CurrentWindowModeSetting.Compare(TEXT("Windowed Fullscreen")) != 0;
}

void ULimenResolutionSetting::ApplyCurrentSetting()
{
	Super::ApplyCurrentSetting();

	GEngine->GetGameUserSettings()->SetScreenResolution(DeFormatResolution(GetCurrentValue()));
	GEngine->GetGameUserSettings()->ApplySettings(false);
}

void ULimenResolutionSetting::SetDefaults()
{	
	DefaultSelection = FormatResolution(GSystemResolution.ResX, GSystemResolution.ResY);
	PossibleSelections.Empty();

	FScreenResolutionArray SupportedResolutions;
	if (GDynamicRHI->RHIGetAvailableResolutions(SupportedResolutions, true))
	{
		for (FScreenResolutionRHI& SupportedResolution : SupportedResolutions)
		{
			PossibleSelections.Push(FormatResolution(SupportedResolution));
		}
	}
	else
	{
		PossibleSelections.Push(DefaultSelection);
	}
}

FString ULimenResolutionSetting::FormatResolution(const FScreenResolutionRHI& InResolution)
{
	return FString::Printf(TEXT("%dx%d"), InResolution.Width, InResolution.Height);
}

FString ULimenResolutionSetting::FormatResolution(const uint32 Width, const uint32 Height)
{
	return FString::Printf(TEXT("%dx%d"), Width, Height);
}

FIntPoint ULimenResolutionSetting::DeFormatResolution(const FString& InResolutionString)
{
	int32 SplitIndex; InResolutionString.FindChar(TEXT('x'), SplitIndex);
	FString Width, Height;
	ULimenString::SplitStringAtIndex(InResolutionString, SplitIndex, Width, Height);
	check(Width.IsNumeric());
	check(Height.IsNumeric());
	
	FIntPoint Res;
	Res.X = FCString::Atoi(*Width);
	Res.Y = FCString::Atoi(*Height);
	return Res;
}

void ULimenResolutionSetting::OnWindowModeUpdated(const ULimenSetting* Setting)
{
	const auto* WindowModeSetting = Cast<ULimenWindowModeSetting>(Setting);
	CurrentWindowModeSetting = WindowModeSetting->GetCurrentValue();
	
	if (CurrentWindowModeSetting.Compare(TEXT("Windowed Fullscreen")) == 0 ||
		LastWindowModeSetting.Compare(TEXT("Windowed Fullscreen")) == 0)
	{
		SetDefaultValue();
		ApplySetting();
		OnSettingEditableStateChanged.Broadcast(this);
	}

	LastWindowModeSetting = WindowModeSetting->GetCurrentValue();
}
