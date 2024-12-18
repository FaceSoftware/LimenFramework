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

void ULimenHardwareRayTracingSetting::ApplyCurrentSetting()
{
	Super::ApplyCurrentSetting();

	const bool HardwareRaytracingEnabled = UnFormatHardwareRayTracingSetting(GetCurrentValue());

	IConsoleVariable* LumenHardwareRayTracingVar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.Lumen.HardwareRayTracing"));
	LumenHardwareRayTracingVar->Set(HardwareRaytracingEnabled, EConsoleVariableFlags::ECVF_SetByGameOverride);

	IConsoleVariable* RayTracedShadowsVar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.RayTracing.Shadows"));
	RayTracedShadowsVar->Set(HardwareRaytracingEnabled, EConsoleVariableFlags::ECVF_SetByGameOverride);
}

void ULimenHardwareRayTracingSetting::SetDefaults()
{
	PossibleSelections.Push(Hardware);
	PossibleSelections.Push(Software);
	DefaultSelection = FormatHardwareRayTracingSetting(GRHIGlobals.RayTracing.Supported);
}

FString ULimenHardwareRayTracingSetting::FormatHardwareRayTracingSetting(const bool bEnabled)
{
	if (bEnabled)
	{
		return Hardware;
	}
	
	return Software;
}

bool ULimenHardwareRayTracingSetting::UnFormatHardwareRayTracingSetting(const FString& Selection)
{
	if (Selection.Compare(Hardware) == 0)
	{
		return true;
	}
	if (Selection.Compare(Software) == 0)
	{
		return false;
	}

	checkNoEntry();
	return false;
}
