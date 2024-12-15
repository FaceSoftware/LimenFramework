// Copyright Face Software. All Rights Reserved.


#include "Settings/LimenRayTracingTwoSidedGeometrySetting.h"

#include "HAL/IConsoleManager.h"


ULimenRayTracingTwoSidedGeometrySetting::ULimenRayTracingTwoSidedGeometrySetting()
{
	DevelopmentName = TEXT("setting_rttwosidedgeometry");
	Category = FText::FromString(TEXT(""));
	DisplayName = FText::FromString(TEXT(""));
	Description = FText::FromString(TEXT(""));
}

bool ULimenRayTracingTwoSidedGeometrySetting::CanEdit() const
{
	return true;
}

void ULimenRayTracingTwoSidedGeometrySetting::ApplyCurrentSetting()
{
	const IConsoleManager* ConsoleManager = &IConsoleManager::Get();
	IConsoleVariable* Variable = ConsoleManager->FindConsoleVariable(TEXT("r.RayTracing.Shadows.EnableTwoSidedGeometry"));

	check(Variable != nullptr);

	Variable->Set(GetCurrentValue(), EConsoleVariableFlags::ECVF_SetByHotfix);
}

void ULimenRayTracingTwoSidedGeometrySetting::SetDefaults()
{
	bDefaultSettingState = false;
}
