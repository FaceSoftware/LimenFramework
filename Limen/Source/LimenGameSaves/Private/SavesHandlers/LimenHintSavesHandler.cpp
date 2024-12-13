// Copyright Face Software. All Rights Reserved.


#include "SavesHandlers/LimenHintSavesHandler.h"

#include "GameplayManagers/LimenSaveableHintsManager.h"


void ULimenHintSavesHandler::LoadDataTo(UWorld* World)
{
	Super::LoadDataTo(World);

	for (TActorIterator<ALimenSaveableHintsManager> It(World); It; ++It)
	{
		HintsManager.LoadData(*It);
	}
}

void ULimenHintSavesHandler::SaveDataFrom(UWorld* World)
{
	Super::SaveDataFrom(World);

	for (TActorIterator<ALimenSaveableHintsManager> It(World); It; ++It)
	{
		HintsManager = FActorSaveData(*It);
	}
}
