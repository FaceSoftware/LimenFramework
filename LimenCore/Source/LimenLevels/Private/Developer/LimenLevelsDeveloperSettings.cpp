// Copyright Face Software. All Rights Reserved.


#include "Developer/LimenLevelsDeveloperSettings.h"

#include "Engine/Level.h"
#include "Engine/World.h"
#include "UObject/Package.h"


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

TSoftObjectPtr<UWorld> ULimenLevelsDeveloperSettings::GetGameLevel(const int32 Index)
{
	const ULimenLevelsDeveloperSettings* Settings = GetDefault<ULimenLevelsDeveloperSettings>();
	check(Settings != nullptr);
	check(Settings->GameLevels.IsValidIndex(Index));

	return Settings->GameLevels[Index];
}

int32 ULimenLevelsDeveloperSettings::GetGameLevelIndex(const FSoftObjectPath& Level)
{
	check(Level.IsValid())
	
	const ULimenLevelsDeveloperSettings* Settings = GetDefault<ULimenLevelsDeveloperSettings>();
	check(Settings != nullptr);
	for (int32 i = 0; i < Settings->GameLevels.Num(); i++)
	{
		if (Settings->GameLevels[i].GetAssetName() == Level.GetAssetName())
		{
			return i;
		}
	}

	return INDEX_NONE;
}

int32 ULimenLevelsDeveloperSettings::GetGameLevelIndex(const UWorld* Level)
{
	return GetGameLevelIndex(FSoftObjectPath(Level));
}

bool ULimenLevelsDeveloperSettings::IsGameLevelIndexValid(const int32 Index)
{
	const ULimenLevelsDeveloperSettings* Settings = GetDefault<ULimenLevelsDeveloperSettings>();
	check(Settings != nullptr);
	return Settings->GameLevels.IsValidIndex(Index);
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
