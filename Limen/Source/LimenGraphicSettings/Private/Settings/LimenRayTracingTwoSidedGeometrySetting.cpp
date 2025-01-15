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

void ULimenRayTracingTwoSidedGeometrySetting::ApplyCurrentSetting(const bool bUserRequest)
{
	Setting.ApplyCVars();
}

void ULimenRayTracingTwoSidedGeometrySetting::SetDefaults()
{
	Setting.AddCVar("r.RayTracing.Shadows.EnableTwoSidedGeometry", false);
}
