// Copyright Face Software. All Rights Reserved.


#include "Developer/LimenLevelsDeveloperSettings.h"


TSoftObjectPtr<UWorld> ULimenLevelsDeveloperSettings::GetInitializationLevel()
{
	const ULimenLevelsDeveloperSettings* Settings = GetDefault<ULimenLevelsDeveloperSettings>();
	check(Settings != nullptr);
	return Settings->InitializationLevel;
}

TSoftObjectPtr<UWorld> ULimenLevelsDeveloperSettings::GetMainMenuLevel()
{
	const ULimenLevelsDeveloperSettings* Settings = GetDefault<ULimenLevelsDeveloperSettings>();
	check(Settings != nullptr);
	return Settings->MainMenuLevel;
}

TSoftObjectPtr<UWorld> ULimenLevelsDeveloperSettings::GetGameEndLevel()
{
	const ULimenLevelsDeveloperSettings* Settings = GetDefault<ULimenLevelsDeveloperSettings>();
	check(Settings != nullptr);
	return Settings->GameEndLevel;
}

TSoftObjectPtr<UWorld> ULimenLevelsDeveloperSettings::GetGameLevel(const uint8 Index)
{
	const ULimenLevelsDeveloperSettings* Settings = GetDefault<ULimenLevelsDeveloperSettings>();
	check(Settings != nullptr);
	check(Settings->GameLevels.IsValidIndex(Index));

	return Settings->GameLevels[Index];
}

uint8 ULimenLevelsDeveloperSettings::GetGameLevelIndex(const UWorld* Level)
{
	check(Level != nullptr);
	
	const ULimenLevelsDeveloperSettings* Settings = GetDefault<ULimenLevelsDeveloperSettings>();
	check(Settings != nullptr);
	for (int32 i = 0; i < Settings->GameLevels.Num(); i++)
	{
		if (Settings->GameLevels[i].ToSoftObjectPath() == FSoftObjectPath(Level))
		{
			return static_cast<uint8>(i);
		}
	}
	
	return INDEX_NONE;
}

bool ULimenLevelsDeveloperSettings::ShouldUseSubsystem()
{
	const ULimenLevelsDeveloperSettings* Settings = GetDefault<ULimenLevelsDeveloperSettings>();
	check(Settings != nullptr);
	return Settings->bUseSubsystem;
}

ULimenLevelsDeveloperSettings::ULimenLevelsDeveloperSettings()
{
	CategoryName = TEXT("Game");
	SectionName = TEXT("Limen - Levels");
	bUseSubsystem = false;
}

FName ULimenLevelsDeveloperSettings::GetContainerName() const
{
	return TEXT("Project");
}

#if WITH_EDITOR
FText ULimenLevelsDeveloperSettings::GetSectionText() const
{
	return FText::FromName(SectionName);
}
#endif
