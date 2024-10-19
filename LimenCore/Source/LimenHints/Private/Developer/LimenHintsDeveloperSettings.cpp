// Copyright Face Software. All Rights Reserved.


#include "Developer/LimenHintsDeveloperSettings.h"

FName ULimenHintsDeveloperSettings::GetContainerName() const
{
	return TEXT("Project");
}

FName ULimenHintsDeveloperSettings::GetCategoryName() const
{
	return TEXT("Game");
}

FName ULimenHintsDeveloperSettings::GetSectionName() const
{
	return TEXT("Limen - Hints");
}

#if WITH_EDITOR
FText ULimenHintsDeveloperSettings::GetSectionText() const
{
	return FText::FromName(GetSectionName());
}

FText ULimenHintsDeveloperSettings::GetSectionDescription() const
{
	return FText::FromString(TEXT("Hints system settings"));
}
#endif
