// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LimenString.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "LimenGraphicSettingsLibrary.generated.h"


namespace LimenSettings
{
	namespace QualityLevels
	{
		static const FText Low = FText::FromString(TEXT("Low"));
		static const FText Medium = FText::FromString(TEXT("Medium"));
		static const FText High = FText::FromString(TEXT("High"));
		static const FText Epic = FText::FromString(TEXT("Epic"));
		static const FText Cinematic = FText::FromString(TEXT("Cinematic"));
		static const FText Custom = FText::FromString(TEXT("Custom"));
	}

	namespace WindowModes
	{
		static const FText Fullscreen = FText::FromString(TEXT("Fullscreen"));
		static const FText WindowedFullscreen = FText::FromString(TEXT("Windowed Fullscreen"));
		static const FText Windowed = FText::FromString(TEXT("Windowed"));
	}
}

USTRUCT(BlueprintType)
struct FResolutionData
{
	GENERATED_BODY()

	explicit FResolutionData(const FIntPoint& InResolution)
	{
		Resolution = InResolution;
		ResolutionTextRepresentation = FString::Printf(TEXT("%dx%d"), Resolution.X, Resolution.Y);
	}

	explicit FResolutionData(const FString& InResolution)
	{
		int32 Index;
		InResolution.FindChar('x', Index);
		check(Index > 0)

		FString Width, Height;
		ULimenString::SplitStringAtIndex(InResolution, Index, Width, Height);

		// Make sure conversion was successful
		check(Width.IsNumeric())
		check(Height.IsNumeric())
		
		Resolution = FIntPoint(FCString::Atoi(*Width), FCString::Atoi(*Height));
		ResolutionTextRepresentation = InResolution;
	}

	UPROPERTY(BlueprintReadOnly)
	FString ResolutionTextRepresentation;

	const FIntPoint& GetResolution() const
	{
		return Resolution;
	}

	/**
	 * @brief Marked public for UHT only. Not intended for use!
	 */
	FResolutionData() = default;

	UPROPERTY(BlueprintReadOnly)
	FIntPoint Resolution = FIntPoint(0);
};

UENUM(BlueprintType)
enum EQualityLevel
{
	Undefined = -2,
	Custom = -1,
	Low = 0,
	Medium = 1,
	High = 2,
	Epic = 3,
	Cinematic = 4,
};

USTRUCT(BlueprintType)
struct FQualityLevelData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)

	FText LevelTextRepresentation;

	explicit FQualityLevelData(const int32 InLevel)
	{
		switch (InLevel)
		{
		case 0:
			LevelTextRepresentation = LimenSettings::QualityLevels::Low;
			Level = EQualityLevel::Low;
			break;

		case 1:
			LevelTextRepresentation = LimenSettings::QualityLevels::Medium;
			Level = EQualityLevel::Medium;
			break;

		case 2:
			LevelTextRepresentation = LimenSettings::QualityLevels::High;
			Level = EQualityLevel::High;
			break;

		case 3:
			LevelTextRepresentation = LimenSettings::QualityLevels::Epic;
			Level = EQualityLevel::Epic;
			break;

		case 4:
			LevelTextRepresentation = LimenSettings::QualityLevels::Cinematic;
			Level = EQualityLevel::Cinematic;
			break;

		case -1:
			LevelTextRepresentation = LimenSettings::QualityLevels::Custom;
			Level = EQualityLevel::Custom;
			break;

		default:
			checkNoEntry()
			break;
		}
	}

	explicit FQualityLevelData(const EQualityLevel InLevel)
	{
		switch (InLevel)
		{
		case EQualityLevel::Low:
			LevelTextRepresentation = LimenSettings::QualityLevels::Low;

			break;

		case EQualityLevel::Medium:
			LevelTextRepresentation = LimenSettings::QualityLevels::Medium;

			break;

		case EQualityLevel::High:
			LevelTextRepresentation = LimenSettings::QualityLevels::High;

			break;

		case EQualityLevel::Epic:
			LevelTextRepresentation = LimenSettings::QualityLevels::Epic;

			break;

		case EQualityLevel::Cinematic:
			LevelTextRepresentation = LimenSettings::QualityLevels::Cinematic;

			break;

		case EQualityLevel::Custom:
			LevelTextRepresentation = LimenSettings::QualityLevels::Custom;
			break;

		default:
			checkNoEntry()
			break;
		}

		Level = InLevel;
	}

	explicit FQualityLevelData(const FString& InLevel)
	{
		if (InLevel == LimenSettings::QualityLevels::Low.ToString())
		{
			Level = Low;
		}
		else if (InLevel == LimenSettings::QualityLevels::Medium.ToString())
		{
			Level = Medium;
		}
		else if (InLevel == LimenSettings::QualityLevels::High.ToString())
		{
			Level = High;
		}
		else if (InLevel == LimenSettings::QualityLevels::Epic.ToString())
		{
			Level = Epic;
		}
		else if (InLevel == LimenSettings::QualityLevels::Cinematic.ToString())
		{
			Level = Cinematic;
		}
		else if (InLevel == LimenSettings::QualityLevels::Custom.ToString())
		{
			Level = Custom;
		}

		if (Level != Undefined)
		{
			LevelTextRepresentation = FText::FromString(InLevel);
		}
	}

	EQualityLevel GetQualityLevel() const
	{
		return Level;
	}

	int32 GetUEQualityLevel() const
	{
		return static_cast<int32>(Level);
	}

	/**
	 * @brief Marked public for UHT only. Not intended for use!
	 */
	FQualityLevelData() = default;

private:
	UPROPERTY(meta=(AllowPrivateAccess=true), BlueprintReadOnly)
	TEnumAsByte<EQualityLevel> Level = EQualityLevel::Undefined;
};

USTRUCT(BlueprintType)
struct FWindowModeData
{
	GENERATED_BODY()
	

	UPROPERTY(BlueprintReadOnly)
	FText WindowModeTextRepresentation;

	explicit FWindowModeData(const EWindowMode::Type InWindowModeType)
	{
		WindowModeType = InWindowModeType;
		switch (WindowModeType)
		{
		case EWindowMode::Fullscreen:
			WindowModeTextRepresentation = LimenSettings::WindowModes::Fullscreen;
			break;

		case EWindowMode::WindowedFullscreen:
			WindowModeTextRepresentation = LimenSettings::WindowModes::WindowedFullscreen;
			break;

		case EWindowMode::Windowed:
			WindowModeTextRepresentation = LimenSettings::WindowModes::Windowed;
			break;

		default:
			checkNoEntry();
		}
	}

	explicit FWindowModeData(const FString& InWindowModeType)
	{
		if (InWindowModeType == LimenSettings::WindowModes::Fullscreen.ToString())
		{
			WindowModeType = EWindowMode::Type::Fullscreen;
			WindowModeTextRepresentation = FText::FromString(InWindowModeType);
		}
		else if (InWindowModeType == LimenSettings::WindowModes::Windowed.ToString())
		{
			WindowModeType = EWindowMode::Type::Windowed;
			WindowModeTextRepresentation = FText::FromString(InWindowModeType);
		}
		else if (InWindowModeType == LimenSettings::WindowModes::WindowedFullscreen.ToString())
		{
			WindowModeType = EWindowMode::Type::WindowedFullscreen;
			WindowModeTextRepresentation = FText::FromString(InWindowModeType);
		}
	}
	
	EWindowMode::Type GetWindowMode() const
	{
		return WindowModeType;
	}

	/**
	 * @brief Marked public for UHT only. Not intended for use!
	 */
	FWindowModeData() = default;

private:
	UPROPERTY(meta=(AllowPrivateAccess=true), BlueprintReadOnly)
	TEnumAsByte<EWindowMode::Type> WindowModeType = static_cast<EWindowMode::Type>(0);
};

/**
 * Static library with helper functions related to graphical options
 */
UCLASS()
class LIMENCORE_API ULimenGraphicSettingsLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	inline static const FWindowModeData FullscreenModeData = FWindowModeData(EWindowMode::Fullscreen);
	inline static const FWindowModeData WindowedModeData = FWindowModeData(EWindowMode::Windowed);
	inline static const FWindowModeData WindowedFullscreenModeData = FWindowModeData(EWindowMode::WindowedFullscreen);
	
public:
	UFUNCTION(BlueprintCallable, Category="Limen|Graphic Settings Library")
	static TArray<FResolutionData> GetSupportedResolutions();

	UFUNCTION(BlueprintCallable, Category="Limen|Graphic Settings Library")
	static FResolutionData GetCurrentResolution();

	UFUNCTION(BlueprintCallable, Category="Limen|Graphic Settings Library")
	static FResolutionData GetDefaultResolution();

	UFUNCTION(BlueprintCallable, Category="Limen|Graphic Settings Library")
	static void SetResolution(const FString& InResolution);

	UFUNCTION(BlueprintCallable, Category="Limen|Graphic Settings Library")
	static TArray<FWindowModeData> GetAllWindowModes();
	
	UFUNCTION(BlueprintCallable, Category="Limen|Graphic Settings Library")
	static const FWindowModeData& GetFullscreenModeData();
	
	UFUNCTION(BlueprintCallable, Category="Limen|Graphic Settings Library")
	static const FWindowModeData& GetWindowedModeData();
	
	UFUNCTION(BlueprintCallable, Category="Limen|Graphic Settings Library")
	static const FWindowModeData& GetWindowedFullscreenModeData();

	UFUNCTION(BlueprintCallable, Category="Limen|Graphic Settings Library")
	static FWindowModeData GetCurrentWindowMode();

	UFUNCTION(BlueprintCallable, Category="Limen|Graphic Settings Library")
	static void SetWindowMode(const FString& InWindowMode);

	UFUNCTION(BlueprintCallable, Category="Limen|Graphic Settings Library")
	static TArray<FQualityLevelData> GetAllQualityLevels();

	UFUNCTION(BlueprintCallable, Category="Limen|Graphic Settings Library")
	static FQualityLevelData GetOverallQualityLevel();

	UFUNCTION(BlueprintCallable, Category="Limen|Graphic Settings Library")
	static void SetOverallQualityLevel(const FString& InQualityLevel);

	UFUNCTION(BlueprintCallable, Category="Limen|Graphic Settings Library")
	static void GetResolutionScale(float& CurrentScaleNormalized, float& CurrentScaleValue, float& MinScaleValue,
	                               float& MaxScaleValue);

	UFUNCTION(BlueprintCallable, Category="Limen|Graphic Settings Library")
	static void SetResolutionScale(const float InResolutionQuality);

	UFUNCTION(BlueprintCallable, Category="Limen|Graphic Settings Library")
	static FQualityLevelData GetViewDistanceQuality();

	UFUNCTION(BlueprintCallable, Category="Limen|Graphic Settings Library")
	static void SetViewDistanceQuality(const FQualityLevelData& InViewDistanceQuality);

	UFUNCTION(BlueprintCallable, Category="Limen|Graphic Settings Library")
	static FQualityLevelData GetAntiAliasingQuality();

	UFUNCTION(BlueprintCallable, Category="Limen|Graphic Settings Library")
	static void SetAntiAliasingQuality(const FQualityLevelData& InAntiAliasingQuality);

	UFUNCTION(BlueprintCallable, Category="Limen|Graphic Settings Library")
	static FQualityLevelData GetShadowQuality();

	UFUNCTION(BlueprintCallable, Category="Limen|Graphic Settings Library")
	static void SetShadowQuality(const FQualityLevelData& InQuality);

	UFUNCTION(BlueprintCallable, Category="Limen|Graphic Settings Library")
	FQualityLevelData GetGlobalIlluminationQuality();

	UFUNCTION(BlueprintCallable, Category="Limen|Graphic Settings Library")
	static void SetGlobalIlluminationQuality(const FQualityLevelData& InQuality);

	UFUNCTION(BlueprintCallable, Category="Limen|Graphic Settings Library")
	static FQualityLevelData GetReflectionQuality();

	UFUNCTION(BlueprintCallable, Category="Limen|Graphic Settings Library")
	static void SetReflectionQuality(const FQualityLevelData& InQuality);

	UFUNCTION(BlueprintCallable, Category="Limen|Graphic Settings Library")
	static FQualityLevelData GetPostProcessQuality();

	UFUNCTION(BlueprintCallable, Category="Limen|Graphic Settings Library")
	static void SetPostProcessQuality(const FQualityLevelData& InQuality);

	UFUNCTION(BlueprintCallable, Category="Limen|Graphic Settings Library")
	static FQualityLevelData GetTextureQuality();

	UFUNCTION(BlueprintCallable, Category="Limen|Graphic Settings Library")
	static void SetTextureQuality(const FQualityLevelData& InQuality);

	UFUNCTION(BlueprintCallable, Category="Limen|Graphic Settings Library")
	static FQualityLevelData GetVisualEffectsQuality();

	UFUNCTION(BlueprintCallable, Category="Limen|Graphic Settings Library")
	static void SetVisualEffectsQuality(const FQualityLevelData& InQuality);

	UFUNCTION(BlueprintCallable, Category="Limen|Graphic Settings Library")
	static FQualityLevelData GetFoliageQuality();

	UFUNCTION(BlueprintCallable, Category="Limen|Graphic Settings Library")
	static void SetFoliageQuality(const FQualityLevelData& InQuality);

	UFUNCTION(BlueprintCallable, Category="Limen|Graphic Settings Library")
	static FQualityLevelData GetShadingQuality();

	UFUNCTION(BlueprintCallable, Category="Limen|Graphic Settings Library")
	static void SetShadingQuality(const FQualityLevelData& InQuality);
	
	UFUNCTION(BlueprintCallable, Category="Limen|Graphic Settings Library")
	static void ApplyAllSettings();

	UFUNCTION(BlueprintCallable, Category="Limen|Graphic Settings Library")
	static void SaveAllSettings();

	UFUNCTION(BlueprintCallable, Category="Limen|Graphic Settings Library")
	static void EnableRayTracing(const bool bEnable);

	UFUNCTION(BlueprintCallable, Category="Limen|Graphic Settings Library", BlueprintPure)
	static bool IsRayTracingEnabled();
	
	UFUNCTION(BlueprintCallable, Category="Limen|Graphic Settings Library", BlueprintPure)
	static bool IsRaytracingSupported();
};
