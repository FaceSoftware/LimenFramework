// Copyright Face Software. All Rights Reserved.


#include "Settings/LimenAntiAliasingSetting.h"


const FString ULimenAntiAliasingSetting::NONE		= TEXT("Disabled");
const FString ULimenAntiAliasingSetting::FXAA		= TEXT("Fast Aproximate Anti-Aliasing (FXAA)");
const FString ULimenAntiAliasingSetting::TAA		= TEXT("Temporal Anti-Aliasing (TAA)");
const FString ULimenAntiAliasingSetting::MSAA		= TEXT("Multi Sample Anti-Aliasing (MSAA)");
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
	
	PossibleSelections.Reserve(7);

	TConsoleSetting<int32> NONESetting(NONE);
	NONESetting.AddCVar("r.AntiAliasingMethod", 0);
	SettingsDescription.Push(NONESetting);
	PossibleSelections.Push(NONE);
	
	TConsoleSetting<int32> FXAASetting(FXAA);
	FXAASetting.AddCVar("r.AntiAliasingMethod", 1);
	FXAASetting.AddCVar("r.FXAA.Quality", 5);
	SettingsDescription.Push(FXAASetting);
	PossibleSelections.Push(FXAA);
	
	TConsoleSetting<int32> TAASetting(TAA);
	TAASetting.AddCVar("r.AntiAliasingMethod", 2);
	TAASetting.AddCVar("r.TemporalAA.Quality", 2);
	SettingsDescription.Push(TAASetting);
	PossibleSelections.Push(TAA);
	
	if (IsMSAASupported())
	{
		TConsoleSetting<int32> MSAASetting(MSAA);
		TAASetting.AddCVar("r.AntiAliasingMethod", 3);
		TAASetting.AddCVar("r.MSAACount", 8);
		SettingsDescription.Push(MSAASetting);
		PossibleSelections.Push(MSAA);
	}
	
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
	PossibleSelections.Push(TSR);

	DefaultSelection = TSR;
}

bool ULimenAntiAliasingSetting::IsMSAASupported() const
{
	static IConsoleVariable* CVarForwardShading = IConsoleManager::Get().FindConsoleVariable(TEXT("r.ForwardShading"));
	return CVarForwardShading && CVarForwardShading->GetInt() != 0;
}
