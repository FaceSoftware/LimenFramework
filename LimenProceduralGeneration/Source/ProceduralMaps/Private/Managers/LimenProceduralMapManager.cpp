// Copyright Face Software. All Rights Reserved.


#include "Managers/LimenProceduralMapManager.h"


void ALimenProceduralMapManager::MapBuilt(UProceduralMapDataAsset* MapData)
{
	check(MapData != nullptr);
}

void ALimenProceduralMapManager::NotifyMapComplete() const
{
	OnMapComplete.Broadcast();
}

void ALimenProceduralMapManager::NotifyMapComplete()
{
	OnMapComplete.Broadcast();
}
