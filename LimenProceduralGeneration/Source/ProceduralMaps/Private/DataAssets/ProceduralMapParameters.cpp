// Copyright Face Software. All Rights Reserved.


#include "DataAssets/ProceduralMapParameters.h"

#include "Managers/LimenProceduralMapManager.h"
#include "MapBuildAlgorithms/LimenMapAlgorithm.h"


TSubclassOf<ALimenProceduralMapManager> UProceduralMapParameters::GetManagerClass() const
{
	return ALimenProceduralMapManager::StaticClass();
}

TSubclassOf<ULimenMapAlgorithm> UProceduralMapParameters::GetGenerationAlgorithm() const
{
	return ULimenMapAlgorithm::StaticClass();
}

bool UProceduralMapParameters::ValidateParameters() const
{
	return true;
}
