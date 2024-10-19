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

TSoftObjectPtr<UWorld> ULimenLevelsDeveloperSettings::GetGameLevel(const uint8 Index)
{
	const ULimenLevelsDeveloperSettings* Settings = GetDefault<ULimenLevelsDeveloperSettings>();
	check(Settings != nullptr);
	check(Settings->GameLevels.IsValidIndex(Index));

	return Settings->GameLevels[Index];
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
