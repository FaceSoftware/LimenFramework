// Copyright © 2024 FaceSoftware. All rights reserved.


#include "Settings/LimenRayTracedShadowsSetting.h"

#include "RHIGlobals.h"
#include "Settings/LimenRayTracingSetting.h"
#include "Subsystems/LimenGraphicalSettingsSubsystem.h"


const FString ULimenRayTracedShadowsSetting::Enabled	= TEXT("Enabled");
const FString ULimenRayTracedShadowsSetting::Disabled	= TEXT("Disabled");

ULimenRayTracedShadowsSetting::ULimenRayTracedShadowsSetting()
{
	DevelopmentName = TEXT("setting_rtshadows");
	Category = FText::FromString(TEXT("Ray-tracing"));
	DisplayName = FText::FromString(TEXT("Raytraced Shadows"));
	Description = FText::FromString(TEXT("Enable or disable ray-traced shadows."));

	bCanEditOverride = false;
}

bool ULimenRayTracedShadowsSetting::CanEdit() const
{
	if (!Super::CanEdit())
	{
		return false;
	}

	if (bCanEditOverride)
	{
		return true;
	}

	IConsoleManager& ConsoleManager = IConsoleManager::Get();
	const IConsoleVariable* RTVar = ConsoleManager.FindConsoleVariable(TEXT("r.RayTracing"));
	return RTVar->GetBool();
}

void ULimenRayTracedShadowsSetting::ApplyCurrentSetting(bool bUserRequest)
{
	Super::ApplyCurrentSetting();

	SettingsDescription.FindByPredicate(
	[this](const TConsoleSetting<bool>& Test)
	{
		return Test.GetName() == GetCurrentValue();
	}
	)->ApplyCVars();
}

void ULimenRayTracedShadowsSetting::SetDefaults()
{
	ULimenSelectionSetting::SetDefaults();

	if (ULimenRayTracingSetting* RTSetting = GetOwnerSubsystem<ULimenGraphicalSettingsSubsystem>()->GetItem<
		ULimenRayTracingSetting>(); RTSetting != nullptr)
	{
		RTSetting->OnSettingApplied.AddUniqueDynamic(this, &ThisClass::RayTracingSettingApplied);
	}

	{
		TConsoleSetting<bool> EnabledSetting(Enabled);
		EnabledSetting.AddCVar(TEXT("r.MegaLights.Allowed"), true);
		EnabledSetting.AddCVar(TEXT("r.RayTracing.Shadows.EnableTwoSidedGeometry"), true);
		SettingsDescription.Push(EnabledSetting);
	}
	{
		TConsoleSetting<bool> DisabledSetting(Disabled);
		DisabledSetting.AddCVar(TEXT("r.MegaLights.Allowed"), false);
		DisabledSetting.AddCVar(TEXT("r.RayTracing.Shadows.EnableTwoSidedGeometry"), false);
		SettingsDescription.Push(DisabledSetting);
	}
	
	PossibleSelections.Push(Enabled);
	PossibleSelections.Push(Disabled);
	DefaultSelection = Enabled;
}

void ULimenRayTracedShadowsSetting::RayTracingSettingApplied(const ULimenSetting* Setting)
{
	const ULimenRayTracingSetting* RTSetting = CastChecked<ULimenRayTracingSetting>(Setting);
	if (RTSetting->GetCurrentValue() == ULimenRayTracingSetting::Disabled)
	{
		bCanEditOverride = true;
		SetNewValue(Disabled);
		ApplyCurrentSetting(false);
		bCanEditOverride = false;
	}
	
	OnSettingEditableStateChanged.Broadcast(Setting);
}
