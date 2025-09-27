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

bool ULimenSavesHandler::SaveDataFrom(UWorld* World)
{
	WorldContext = World;
	return WorldContext.IsValid();
}

bool ULimenSavesHandler::LoadDataTo(UWorld* World)
{
	WorldContext = World;
	return WorldContext.IsValid();
}

UWorld* ULimenSavesHandler::GetWorld() const
{
	ensureMsgf(WorldContext.IsValid(), TEXT("ULimenSavesHandler::GetWorld() called before SaveDataFrom/LoadDataTo set WorldContext."));
	return WorldContext.Get();
}
