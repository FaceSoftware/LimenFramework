// Copyright Face Software. All Rights Reserved.


#include "Settings/LimenHardwareRayTracingSetting.h"


ULimenHardwareRayTracingSetting::ULimenHardwareRayTracingSetting()
{
	DevelopmentName = TEXT("setting_lumenhrt");
	Category = FText::FromString(TEXT("Graphics"));
	DisplayName = FText::FromString(TEXT("Lumen Hardware Raytracing"));
	Description = FText::FromString(TEXT("Whether Lumen should use hardware raytracing or software raytracing (needs a raytracing-capable graphics card and Lumen as the global illumination method)."));
}

bool ULimenHardwareRayTracingSetting::CanEdit() const
{
	if (!Super::CanEdit())
	{
		return false;
	}

	return GRHISupportsRayTracing;
}

void ULimenHardwareRayTracingSetting::ApplyCurrentSetting()
{
	Super::ApplyCurrentSetting();

	IConsoleVariable* LumenHardwareRayTracingVar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.Lumen.HardwareRayTracing"));
	LumenHardwareRayTracingVar->Set(UnFormatHardwareRayTracingSetting(GetCurrentValue()));
}

void ULimenHardwareRayTracingSetting::SetDefaults()
{
	PossibleSelections.Push(Enabled);
	PossibleSelections.Push(Disabled);
	DefaultSelection = FormatHardwareRayTracingSetting(GRHISupportsRayTracing);
}

FString ULimenHardwareRayTracingSetting::FormatHardwareRayTracingSetting(const bool bEnabled)
{
	if (bEnabled)
	{
		return Enabled;
	}
	
	return Disabled;
}

bool ULimenHardwareRayTracingSetting::UnFormatHardwareRayTracingSetting(const FString& Selection)
{
	if (Selection.Compare(Enabled) == 0)
	{
		return true;
	}
	if (Selection.Compare(Disabled) == 0)
	{
		return false;
	}

	checkNoEntry();
	return false;
}
