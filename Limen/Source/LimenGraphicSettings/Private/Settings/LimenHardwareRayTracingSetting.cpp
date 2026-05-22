// Copyright Face Software. All Rights Reserved.


#include "Settings/LimenHardwareRayTracingSetting.h"

#include "RHIGlobals.h"
#include "HAL/IConsoleManager.h"
#include "Settings/LimenRayTracingSetting.h"
#include "Subsystems/LimenGraphicalSettingsSubsystem.h"


const FString ULimenHardwareRayTracingSetting::Software = TEXT("Software");
const FString ULimenHardwareRayTracingSetting::Hardware = TEXT("Hardware");

ULimenHardwareRayTracingSetting::ULimenHardwareRayTracingSetting()
{
	DevelopmentName = TEXT("setting_lumenhrt");
	Category = FText::FromString(TEXT("Ray-tracing"));
	DisplayName = FText::FromString(TEXT("Raytracing Method"));
	Description = FText::FromString(TEXT("Whether hardware or software raytracing should be used for global illumination."));

	bCanEditOverride = false;
}

bool ULimenHardwareRayTracingSetting::CanEdit() const
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

void ULimenHardwareRayTracingSetting::ApplyCurrentSetting(const bool bUserRequest)
{
	Super::ApplyCurrentSetting();

	SettingsDescription.FindByPredicate(
	[this](const TConsoleSetting<bool>& Test)
	{
		return Test.GetName() == GetCurrentValue();
	}
	)->ApplyCVars();
}

void ULimenHardwareRayTracingSetting::SetDefaults()
{
	ULimenSelectionSetting::SetDefaults();

	if (ULimenRayTracingSetting* RTSetting = GetOwnerSubsystem<ULimenGraphicalSettingsSubsystem>()->GetItem<
		ULimenRayTracingSetting>(); RTSetting != nullptr)
	{
		RTSetting->OnSettingApplied.AddUniqueDynamic(this, &ThisClass::RayTracingSettingApplied);
	}

	{
		TConsoleSetting<bool> HRT(Hardware);
		HRT.AddCVar(TEXT("r.Lumen.HardwareRayTracing"), true);
		SettingsDescription.Push(HRT);
	}
	{
		TConsoleSetting<bool> SRT(Software);
		SRT.AddCVar(TEXT("r.Lumen.HardwareRayTracing"), false);
		SettingsDescription.Push(SRT);
	}
	
	PossibleSelections.Push(Hardware);
	PossibleSelections.Push(Software);
	DefaultSelection = GRHIGlobals.RayTracing.Supported ? Hardware : Software;
}

void ULimenHardwareRayTracingSetting::RayTracingSettingApplied(const ULimenSetting* Setting)
{
	const ULimenRayTracingSetting* RTSetting = CastChecked<ULimenRayTracingSetting>(Setting);
	if (RTSetting->GetCurrentValue() == ULimenRayTracingSetting::Disabled)
	{
		bCanEditOverride = true;
		SetNewValue(Software);
		ApplyCurrentSetting(false);
		bCanEditOverride = false;
	}

	OnSettingEditableStateChanged.Broadcast(this);
}
