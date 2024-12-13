// Copyright Face Software. All Rights Reserved.


#include "Developer/LimenGameSavesDeveloperSettings.h"

FName ULimenGameSavesDeveloperSettings::GetContainerName() const
{
	return TEXT("Project");
}

FName ULimenGameSavesDeveloperSettings::GetCategoryName() const
{
	return TEXT("Game");
}

FName ULimenGameSavesDeveloperSettings::GetSectionName() const
{
	return TEXT("Limen - Game Saves");
}

#if WITH_EDITOR
FText ULimenGameSavesDeveloperSettings::GetSectionText() const
{
	return FText::FromName(TEXT("Limen - Game Saves"));
}

FText ULimenGameSavesDeveloperSettings::GetSectionDescription() const
{
	return FText::FromName(TEXT("Settings for the game save system"));
}
#endif
