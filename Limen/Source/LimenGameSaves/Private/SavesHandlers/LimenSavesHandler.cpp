// Copyright Face Software. All Rights Reserved.


#include "SavesHandlers/LimenSavesHandler.h"

bool ULimenSavesHandler::ShouldSaveData() const
{
	return true;
}

bool ULimenSavesHandler::ShouldLoadData() const
{
	return true;
}

void ULimenSavesHandler::DataSaved()
{
}

void ULimenSavesHandler::DataLoaded()
{
}
