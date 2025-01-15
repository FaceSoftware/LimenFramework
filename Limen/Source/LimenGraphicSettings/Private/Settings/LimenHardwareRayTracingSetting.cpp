// Copyright Face Software. All Rights Reserved.


#include "Settings/LimenHardwareRayTracingSetting.h"

#include "RHIGlobals.h"
#include "HAL/IConsoleManager.h"


const FString ULimenHardwareRayTracingSetting::Software = TEXT("Software");
const FString ULimenHardwareRayTracingSetting::Hardware = TEXT("Hardware");

ULimenHardwareRayTracingSetting::ULimenHardwareRayTracingSetting()
{
	DevelopmentName = TEXT("setting_lumenhrt");
	Category = FText::FromString(TEXT("Graphics"));
	DisplayName = FText::FromString(TEXT("Raytracing Method"));
	Description = FText::FromString(TEXT("Whether hardware or software raytracing should be used."));
}

bool ULimenHardwareRayTracingSetting::CanEdit() const
{
	if (!Super::CanEdit())
	{
		return false;
	}

	return GRHIGlobals.RayTracing.Supported;
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
	TConsoleSetting<bool> HRT(Hardware);
	HRT.AddCVar("r.Lumen.HardwareRayTracing", true);
	HRT.AddCVar("r.RayTracing.Shadows", true);
	SettingsDescription.Push(HRT);
	
	TConsoleSetting<bool> SRT(Software);
	SRT.AddCVar("r.Lumen.HardwareRayTracing", false);
	SRT.AddCVar("r.RayTracing.Shadows", false);
	SettingsDescription.Push(SRT);
	
	PossibleSelections.Push(Hardware);
	PossibleSelections.Push(Software);
	DefaultSelection = GRHIGlobals.RayTracing.Supported ? Hardware : Software;
}
