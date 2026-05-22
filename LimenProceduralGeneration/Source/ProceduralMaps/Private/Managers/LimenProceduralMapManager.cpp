// Copyright Face Software. All Rights Reserved.


#include "Managers/LimenProceduralMapManager.h"


void ALimenProceduralMapManager::MapBuilt(ULimenProceduralMap* Map)
{
	check(Map != nullptr);
}

void ALimenProceduralMapManager::NotifyMapComplete()
{
	OnMapComplete.Broadcast();
}

void ALimenProceduralMapManager::NotifyMapEntered()
{
	OnMapEntered.Broadcast();
}

ALimenProceduralMapManager::FMapComplete& ALimenProceduralMapManager::GetMapCompleteDelegate()
{
	return OnMapComplete;
}

ALimenProceduralMapManager::FMapEnter& ALimenProceduralMapManager::GetMapEnteredDelegate()
{
	return OnMapEntered;
}
