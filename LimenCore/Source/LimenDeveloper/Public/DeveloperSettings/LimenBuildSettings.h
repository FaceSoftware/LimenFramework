// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "Misc/ConfigCacheIni.h"
#include "LimenBuildSettings.generated.h"


UCLASS(Config="Limen")
class LIMENDEVELOPER_API ULimenBuildSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	/**
	 * @brief Path to a text file that **only** contains the build counter number
	 */
	UPROPERTY(EditAnywhere, Config)
	FFilePath BuildCounterFilePath;

	UPROPERTY(EditAnywhere, Config)
	FString DevelopmentStage;
	
	ULimenBuildSettings()
	{
		CategoryName = TEXT("Project");
		SectionName = TEXT("Limen - Packaging");
		
#if WITH_EDITOR
		OnSettingChanged().AddLambda([this] (UObject* Object, struct FPropertyChangedEvent& Event)
		{
			// Keep string updated
			BuildInfoString = GetBuildInfo();
			WriteBuildInfoToConfig();
		});
#endif
	}

	virtual ~ULimenBuildSettings() override
	{
		// Keep string updated
		BuildInfoString = GetBuildInfo();
		WriteBuildInfoToConfig();
	}

#if WITH_EDITOR
	virtual FText GetSectionText() const override
	{
		return FText::FromName(SectionName);
	}
#endif
	
	virtual FName GetContainerName() const override { return TEXT("Project"); }

	UFUNCTION(BlueprintCallable, Category="Limen|Developer")
	static FString GetBuildInfo()
	{
		const ULimenBuildSettings* Settings = GetDefault<ULimenBuildSettings>();
		
#if !WITH_EDITOR
		return Settings->BuildInfoString;
#endif

		const FString FullFilePath = FPaths::ConvertRelativePathToFull(Settings->BuildCounterFilePath.FilePath);
		FString TempBuildNumber;
		if (!FFileHelper::LoadFileToString(TempBuildNumber, *FullFilePath))
		{
			return FString(TEXT(""));
		}

		FString BuildNumber; 
		for (int32 i = 0; i < TempBuildNumber.Len(); i++)
		{
			const TCHAR Letter = TempBuildNumber[i];
			if (FChar::IsDigit(Letter))
			{
				BuildNumber += Letter;
			}
		}

		FString ProjectVersion;
		GConfig->GetString(
				TEXT("/Script/EngineSettings.GeneralProjectSettings"),
				TEXT("ProjectVersion"),
				ProjectVersion,
				GGameIni
			);

		FString ProjectName;
		GConfig->GetString(
				TEXT("/Script/EngineSettings.GeneralProjectSettings"),
				TEXT("ProjectName"),
				ProjectName,
				GGameIni
			);

		FString DevelopmentStage = Settings->DevelopmentStage;
		if (Settings->DevelopmentStage.IsEmpty())
		{
			DevelopmentStage = TEXT("stage");
		}

		switch (FApp::GetBuildConfiguration())
		{			
		case EBuildConfiguration::Debug:
			return FString::Printf(TEXT("%s_%s_%s.%s_debug_build"), *ProjectName, *DevelopmentStage, *ProjectVersion, *BuildNumber);
			
		case EBuildConfiguration::DebugGame:
			return FString::Printf(TEXT("%s_%s_%s.%s_debuggame_build"), *ProjectName, *DevelopmentStage, *ProjectVersion, *BuildNumber);
			
		case EBuildConfiguration::Development:
			return FString::Printf(TEXT("%s_%s_%s.%s_development_build"), *ProjectName, *DevelopmentStage, *ProjectVersion, *BuildNumber);
			
		case EBuildConfiguration::Shipping:
			return FString::Printf(TEXT("%s_%s_%s.%s_shipping_build"), *ProjectName, *DevelopmentStage, *ProjectVersion, *BuildNumber);
			
		case EBuildConfiguration::Test:
			return FString::Printf(TEXT("%s_%s_%s.%s_test_build"), *ProjectName, *DevelopmentStage, *ProjectVersion, *BuildNumber);

		default:
			break;
		}
		
		return FString::Printf(TEXT("%s_%s_%s.%s"), *ProjectName, *DevelopmentStage, *ProjectVersion, *BuildNumber);
	}

private:
	UPROPERTY(Config)
	FString BuildInfoString;

	void WriteBuildInfoToConfig()
	{
		const FString LimenCoreIniFilePath = FPaths::ProjectConfigDir() + TEXT("DefaultLimen.ini");
		GConfig->SetString(
			TEXT("/Script/LimenDeveloper.LimenBuildSettings"),
			TEXT("BuildInfoString"),
			*BuildInfoString,
			LimenCoreIniFilePath
			);
		
		GConfig->Flush(false, LimenCoreIniFilePath);
	}

	void ReadBuildInfoFromConfig()
	{
		const FString LimenCoreIniFilePath = FPaths::ProjectConfigDir() + TEXT("DefaultLimen.ini");
		GConfig->GetString(
			TEXT("/Script/LimenDeveloper.LimenBuildSettings"),
			TEXT("BuildInfoString"),
			BuildInfoString,
			LimenCoreIniFilePath
			);
	}
};
