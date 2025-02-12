// Copyright Face Software. All Rights Reserved.


#include "SavesHandlers/LimenHintSavesHandler.h"

#include "GameplayManagers/LimenSaveableHintsManager.h"


bool ULimenHintSavesHandler::LoadDataTo(UWorld* World)
{
	Super::LoadDataTo(World);

	for (TActorIterator<ALimenSaveableHintsManager> It(World); It; ++It)
	{
		HintsManager.LoadData(*It);
	}

	return true;
}

bool ULimenHintSavesHandler::SaveDataFrom(UWorld* World)
{
	Super::SaveDataFrom(World);

	for (TActorIterator<ALimenSaveableHintsManager> It(World); It; ++It)
	{
		HintsManager = FActorSaveData(*It);
	}

	return true;
}
