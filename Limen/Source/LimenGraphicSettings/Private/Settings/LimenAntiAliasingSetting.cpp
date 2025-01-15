// Copyright Face Software. All Rights Reserved.


#include "Settings/LimenAntiAliasingSetting.h"

#include "Subsystems/LimenGraphicalSettingsSubsystem.h"
#include "Subsystems/LimenModularSettingsSubsystem.h"


const FString ULimenAntiAliasingSetting::NONE		= TEXT("Disabled");
const FString ULimenAntiAliasingSetting::FXAA		= TEXT("Fast Aproximate Anti-Aliasing (FXAA)");
const FString ULimenAntiAliasingSetting::TAA		= TEXT("Temporal Anti-Aliasing (TAA)");
const FString ULimenAntiAliasingSetting::TSR		= TEXT("Temporal Super Resolution (TSR)");

ULimenAntiAliasingSetting::ULimenAntiAliasingSetting()
{
	DevelopmentName = TEXT("setting_antialiasing");
	Category = FText::FromString(TEXT("Graphics"));
	DisplayName = FText::FromString(TEXT("Anti Aliasing Method"));
	Description = FText::FromString(TEXT("Anti-aliasing reduces visual artifacts such as jagged edges by smoothing the transitions between contrasting pixels."));
}

void ULimenAntiAliasingSetting::ApplyCurrentSetting(const bool bUserRequest)
{
	Super::ApplyCurrentSetting();

	const FString CurrentSetting = GetCurrentValue();
	TConsoleSetting<int32>* SettingDescription = SettingsDescription.FindByPredicate(
	[this, &CurrentSetting] (const TConsoleSetting<int32>& Test)
	{
		return Test.GetName() == CurrentSetting;
	});
	check(SettingDescription != nullptr)
	SettingDescription->ApplyCVars();
}

void ULimenAntiAliasingSetting::SetDefaults()
{
	Super::SetDefaults();

	TConsoleSetting<int32> NONESetting(NONE);
	NONESetting.AddCVar("r.AntiAliasingMethod", 0);
	SettingsDescription.Push(NONESetting);
	
	TConsoleSetting<int32> FXAASetting(FXAA);
	FXAASetting.AddCVar("r.AntiAliasingMethod", 1);
	FXAASetting.AddCVar("r.FXAA.Quality", 5);
	SettingsDescription.Push(FXAASetting);
	
	TConsoleSetting<int32> TAASetting(TAA);
	TAASetting.AddCVar("r.AntiAliasingMethod", 2);
	TAASetting.AddCVar("r.TemporalAA.Quality", 2);
	SettingsDescription.Push(TAASetting);
	
	TConsoleSetting<int32> TSRSetting(TSR);
	TSRSetting.AddCVar("r.AntiAliasingMethod", 4);
	TSRSetting.AddCVar("r.TemporalAA.Quality", 2);
	TSRSetting.AddCVar("r.TSR.History.R11G11B10", 1);
	TSRSetting.AddCVar("r.TSR.History.ScreenPercentage", 100);
	TSRSetting.AddCVar("r.TSR.History.UpdateQuality", 2);
	TSRSetting.AddCVar("r.TSR.ShadingRejection.Flickering", 1);
	TSRSetting.AddCVar("r.TSR.RejectionAntiAliasingQuality", 1);
	TSRSetting.AddCVar("r.TSR.Resurrection", 1);
	SettingsDescription.Push(TSRSetting);

	PossibleSelections.Reserve(7);
	PossibleSelections.Push(NONE);
	PossibleSelections.Push(FXAA);
	PossibleSelections.Push(TAA);
	PossibleSelections.Push(TSR);

	DefaultSelection = TAA;
}
