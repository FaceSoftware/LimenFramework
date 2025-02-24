// Copyright © 2024 FaceSoftware. All rights reserved.


#include "Settings/LimenRayTracingSetting.h"

#include "RHIGlobals.h"


const FString ULimenRayTracingSetting::Enabled	= TEXT("Enabled");
const FString ULimenRayTracingSetting::Disabled	= TEXT("Disabled");

ULimenRayTracingSetting::ULimenRayTracingSetting()
{
	DevelopmentName = TEXT("setting_rt");
	Category = FText::FromString(TEXT("Ray-tracing"));
	DisplayName = FText::FromString(TEXT("Ray-tracing"));
	Description = FText::FromString(TEXT("Enable or disable raytracing."));
}

bool ULimenRayTracingSetting::CanEdit() const
{
	if (!Super::CanEdit())
	{
		return false;
	}

	return GRHIGlobals.RayTracing.Supported;
}

void ULimenRayTracingSetting::ApplyCurrentSetting(bool bUserRequest)
{
	Super::ApplyCurrentSetting();

	SettingsDescription.FindByPredicate(
	[this](const TConsoleSetting<bool>& Test)
	{
		return Test.GetName() == GetCurrentValue();
	}
	)->ApplyCVars();
}

void ULimenRayTracingSetting::SetDefaults()
{
	ULimenSelectionSetting::SetDefaults();

	{
		TConsoleSetting<bool> EnabledSetting(Enabled);
		EnabledSetting.AddCVar(TEXT("r.RayTracing"), true);
		SettingsDescription.Push(EnabledSetting);
	}
	{
		TConsoleSetting<bool> DisabledSetting(Disabled);
		DisabledSetting.AddCVar(TEXT("r.RayTracing"), false);
		SettingsDescription.Push(DisabledSetting);
	}
	
	PossibleSelections.Push(Enabled);
	PossibleSelections.Push(Disabled);
	DefaultSelection = GRHIGlobals.RayTracing.Supported ? Enabled : Disabled;
}
