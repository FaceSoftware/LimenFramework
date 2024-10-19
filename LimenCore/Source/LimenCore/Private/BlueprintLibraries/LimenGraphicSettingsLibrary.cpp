// Copyright Face Software. All Rights Reserved.


#include "BlueprintLibraries/LimenGraphicSettingsLibrary.h"

#include "GameFramework/GameUserSettings.h"
#include "Kismet/GameplayStatics.h"
#include "RHI.h"

TArray<FResolutionData> ULimenGraphicSettingsLibrary::GetSupportedResolutions()
{
	TArray<FIntPoint> SupportedResolutions;
	if (!UKismetSystemLibrary::GetSupportedFullscreenResolutions(SupportedResolutions))
	{
		return TArray<FResolutionData>();
	}

	TArray<FResolutionData> OutData;
	for (auto& Resolution : SupportedResolutions)
	{
		OutData.Push(FResolutionData(Resolution));
	}
	
	return OutData;
}

FResolutionData ULimenGraphicSettingsLibrary::GetCurrentResolution()
{
	check(GEngine)
	const auto* UserSettings = GEngine->GetGameUserSettings();
	check(UserSettings)

	return FResolutionData(UserSettings->GetLastConfirmedScreenResolution());
}

FResolutionData ULimenGraphicSettingsLibrary::GetDefaultResolution()
{	
	check(GEngine)
	const auto* UserSettings = GEngine->GetGameUserSettings();
	check(UserSettings)

	return FResolutionData(UserSettings->GetDefaultResolution());
}

void ULimenGraphicSettingsLibrary::SetResolution(const FString& InResolution)
{
	const FResolutionData Resolution(InResolution);
	
	check(GEngine)
	auto* UserSettings = GEngine->GetGameUserSettings();
	check(UserSettings)

	UserSettings->SetScreenResolution(Resolution.GetResolution());
}

TArray<FWindowModeData> ULimenGraphicSettingsLibrary::GetAllWindowModes()
{
	TArray<FWindowModeData> OutData;
	OutData.Push(FWindowModeData(EWindowMode::Fullscreen));
	OutData.Push(FWindowModeData(EWindowMode::WindowedFullscreen));
	OutData.Push(FWindowModeData(EWindowMode::Windowed));
	return OutData;
}

const FWindowModeData& ULimenGraphicSettingsLibrary::GetFullscreenModeData()
{
	return FullscreenModeData;
}

const FWindowModeData& ULimenGraphicSettingsLibrary::GetWindowedModeData()
{
	return WindowedModeData;
}

const FWindowModeData& ULimenGraphicSettingsLibrary::GetWindowedFullscreenModeData()
{
	return WindowedFullscreenModeData;
}

FWindowModeData ULimenGraphicSettingsLibrary::GetCurrentWindowMode()
{
	check(GEngine)
	const auto* UserSettings = GEngine->GetGameUserSettings();
	check(UserSettings)
	
	return FWindowModeData(UserSettings->GetFullscreenMode());
}

void ULimenGraphicSettingsLibrary::SetWindowMode(const FString& InWindowMode)
{
	check(GEngine)
	auto* UserSettings = GEngine->GetGameUserSettings();
	check(UserSettings)

	UserSettings->SetFullscreenMode(FWindowModeData(InWindowMode).GetWindowMode());
}

TArray<FQualityLevelData> ULimenGraphicSettingsLibrary::GetAllQualityLevels()
{
	TArray<FQualityLevelData> OutData;
	OutData.Push(FQualityLevelData(EQualityLevel::Low));
	OutData.Push(FQualityLevelData(EQualityLevel::Medium));
	OutData.Push(FQualityLevelData(EQualityLevel::High));
	OutData.Push(FQualityLevelData(EQualityLevel::Epic));
	OutData.Push(FQualityLevelData(EQualityLevel::Cinematic));
	OutData.Push(FQualityLevelData(EQualityLevel::Custom));
	return OutData;
}

FQualityLevelData ULimenGraphicSettingsLibrary::GetOverallQualityLevel()
{
	check(GEngine)
	const auto* UserSettings = GEngine->GetGameUserSettings();
	check(UserSettings)
	
	return FQualityLevelData(UserSettings->GetOverallScalabilityLevel());
}

void ULimenGraphicSettingsLibrary::SetOverallQualityLevel(const FString& InQualityLevel)
{
	check(GEngine)
	auto* UserSettings = GEngine->GetGameUserSettings();
	check(UserSettings)

	const FQualityLevelData InQualityLevelData(InQualityLevel);

	for (auto& Level : GetAllQualityLevels())
	{
		if (InQualityLevelData.GetQualityLevel() == Level.GetQualityLevel())
		{
			UserSettings->SetOverallScalabilityLevel(InQualityLevelData.GetUEQualityLevel());
		}
	}
}

void ULimenGraphicSettingsLibrary::GetResolutionScale(float& CurrentScaleNormalized, float& CurrentScaleValue, float& MinScaleValue, float& MaxScaleValue)
{
	check(GEngine)
	const auto* UserSettings = GEngine->GetGameUserSettings();
	check(UserSettings)

	UserSettings->GetResolutionScaleInformationEx(CurrentScaleNormalized, CurrentScaleValue, MinScaleValue, MaxScaleValue);
}

void ULimenGraphicSettingsLibrary::SetResolutionScale(const float InResolutionQuality)
{
	check(GEngine)
	auto* UserSettings = GEngine->GetGameUserSettings();
	check(UserSettings)

	UserSettings->SetResolutionScaleValueEx(InResolutionQuality);
}

FQualityLevelData ULimenGraphicSettingsLibrary::GetViewDistanceQuality()
{
	check(GEngine)
	const auto* UserSettings = GEngine->GetGameUserSettings();
	check(UserSettings)

	return FQualityLevelData(UserSettings->GetViewDistanceQuality());
}

void ULimenGraphicSettingsLibrary::SetViewDistanceQuality(const FQualityLevelData& InViewDistanceQuality)
{
	check(GEngine)
	auto* UserSettings = GEngine->GetGameUserSettings();
	check(UserSettings)

	UserSettings->SetViewDistanceQuality(InViewDistanceQuality.GetUEQualityLevel());
}

FQualityLevelData ULimenGraphicSettingsLibrary::GetAntiAliasingQuality()
{
	check(GEngine)
	const auto* UserSettings = GEngine->GetGameUserSettings();
	check(UserSettings)

	return FQualityLevelData(UserSettings->GetAntiAliasingQuality());
}

void ULimenGraphicSettingsLibrary::SetAntiAliasingQuality(const FQualityLevelData& InAntiAliasingQuality)
{
	check(GEngine)
	auto* UserSettings = GEngine->GetGameUserSettings();
	check(UserSettings)

	UserSettings->SetAntiAliasingQuality(InAntiAliasingQuality.GetUEQualityLevel());
}

FQualityLevelData ULimenGraphicSettingsLibrary::GetShadowQuality()
{
	check(GEngine)
	const auto* UserSettings = GEngine->GetGameUserSettings();
	check(UserSettings)

	return FQualityLevelData(UserSettings->GetShadowQuality());
}

void ULimenGraphicSettingsLibrary::SetShadowQuality(const FQualityLevelData& InQuality)
{
	check(GEngine)
	auto* UserSettings = GEngine->GetGameUserSettings();
	check(UserSettings)

	UserSettings->SetShadowQuality(InQuality.GetUEQualityLevel());
}

FQualityLevelData ULimenGraphicSettingsLibrary::GetGlobalIlluminationQuality()
{
	check(GEngine)
	const auto* UserSettings = GEngine->GetGameUserSettings();
	check(UserSettings)

	return FQualityLevelData(UserSettings->GetGlobalIlluminationQuality());
}

void ULimenGraphicSettingsLibrary::SetGlobalIlluminationQuality(const FQualityLevelData& InQuality)
{
	check(GEngine)
	auto* UserSettings = GEngine->GetGameUserSettings();
	check(UserSettings)

	UserSettings->SetGlobalIlluminationQuality(InQuality.GetUEQualityLevel());
}

FQualityLevelData ULimenGraphicSettingsLibrary::GetReflectionQuality()
{
	check(GEngine)
	const auto* UserSettings = GEngine->GetGameUserSettings();
	check(UserSettings)

	return FQualityLevelData(UserSettings->GetReflectionQuality());
}

void ULimenGraphicSettingsLibrary::SetReflectionQuality(const FQualityLevelData& InQuality)
{
	check(GEngine)
	auto* UserSettings = GEngine->GetGameUserSettings();
	check(UserSettings)

	UserSettings->SetReflectionQuality(InQuality.GetUEQualityLevel());
}

FQualityLevelData ULimenGraphicSettingsLibrary::GetPostProcessQuality()
{
	check(GEngine)
	const auto* UserSettings = GEngine->GetGameUserSettings();
	check(UserSettings)

	return FQualityLevelData(UserSettings->GetPostProcessingQuality());
}

void ULimenGraphicSettingsLibrary::SetPostProcessQuality(const FQualityLevelData& InQuality)
{
	check(GEngine)
	auto* UserSettings = GEngine->GetGameUserSettings();
	check(UserSettings)

	UserSettings->SetPostProcessingQuality(InQuality.GetUEQualityLevel());
}

FQualityLevelData ULimenGraphicSettingsLibrary::GetTextureQuality()
{
	check(GEngine)
	const auto* UserSettings = GEngine->GetGameUserSettings();
	check(UserSettings)

	return FQualityLevelData(UserSettings->GetTextureQuality());
}

void ULimenGraphicSettingsLibrary::SetTextureQuality(const FQualityLevelData& InQuality)
{
	check(GEngine)
	auto* UserSettings = GEngine->GetGameUserSettings();
	check(UserSettings)

	UserSettings->SetTextureQuality(InQuality.GetUEQualityLevel());
}

FQualityLevelData ULimenGraphicSettingsLibrary::GetVisualEffectsQuality()
{
	check(GEngine)
	const auto* UserSettings = GEngine->GetGameUserSettings();
	check(UserSettings)

	return FQualityLevelData(UserSettings->GetVisualEffectQuality());
}

void ULimenGraphicSettingsLibrary::SetVisualEffectsQuality(const FQualityLevelData& InQuality)
{
	check(GEngine)
	auto* UserSettings = GEngine->GetGameUserSettings();
	check(UserSettings)

	UserSettings->SetVisualEffectQuality(InQuality.GetUEQualityLevel());
}

FQualityLevelData ULimenGraphicSettingsLibrary::GetFoliageQuality()
{
	check(GEngine)
	const auto* UserSettings = GEngine->GetGameUserSettings();
	check(UserSettings)

	return FQualityLevelData(UserSettings->GetFoliageQuality());
}

void ULimenGraphicSettingsLibrary::SetFoliageQuality(const FQualityLevelData& InQuality)
{
	check(GEngine)
	auto* UserSettings = GEngine->GetGameUserSettings();
	check(UserSettings)

	UserSettings->SetFoliageQuality(InQuality.GetUEQualityLevel());
}

FQualityLevelData ULimenGraphicSettingsLibrary::GetShadingQuality()
{
	check(GEngine)
	const auto* UserSettings = GEngine->GetGameUserSettings();
	check(UserSettings)

	return FQualityLevelData(UserSettings->GetShadingQuality());
}

void ULimenGraphicSettingsLibrary::SetShadingQuality(const FQualityLevelData& InQuality)
{
	check(GEngine)
	auto* UserSettings = GEngine->GetGameUserSettings();
	check(UserSettings)

	UserSettings->SetShadingQuality(InQuality.GetUEQualityLevel());
}

void ULimenGraphicSettingsLibrary::ApplyAllSettings()
{
	check(GEngine)
	auto* UserSettings = GEngine->GetGameUserSettings();
	check(UserSettings)

	UserSettings->ApplySettings(false);
	UserSettings->ApplyResolutionSettings(false);
}

void ULimenGraphicSettingsLibrary::SaveAllSettings()
{
	check(GEngine)
	auto* UserSettings = GEngine->GetGameUserSettings();
	check(UserSettings)

	UserSettings->SaveSettings();
}

void ULimenGraphicSettingsLibrary::EnableRayTracing(const bool bEnable)
{
	static IConsoleVariable* CVarRayTracing = IConsoleManager::Get().FindConsoleVariable(TEXT("r.RayTracing"));
	check(CVarRayTracing != nullptr);
	CVarRayTracing->Set(bEnable);
}

bool ULimenGraphicSettingsLibrary::IsRayTracingEnabled()
{
	static IConsoleVariable* CVarRayTracing = IConsoleManager::Get().FindConsoleVariable(TEXT("r.RayTracing"));
	check(CVarRayTracing != nullptr);
	return CVarRayTracing->GetBool();
}

bool ULimenGraphicSettingsLibrary::IsRaytracingSupported()
{
	return GRHISupportsRayTracing;
}