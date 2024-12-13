// Copyright Face Software. All Rights Reserved.


#include "Developer/LimenModularSavesDeveloperSettings.h"

FName ULimenModularSavesDeveloperSettings::GetContainerName() const
{
	return TEXT("Project");
}

FName ULimenModularSavesDeveloperSettings::GetCategoryName() const
{
	return TEXT("Game");
}

FName ULimenModularSavesDeveloperSettings::GetSectionName() const
{
	return TEXT("Limen - Game Saves");
}

#if WITH_EDITOR
FText ULimenModularSavesDeveloperSettings::GetSectionText() const
{
	return FText::FromName(TEXT("Limen - Game Saves"));
}

FText ULimenModularSavesDeveloperSettings::GetSectionDescription() const
{
	return FText::FromName(TEXT("Settings for the game save system"));
}
#endif
