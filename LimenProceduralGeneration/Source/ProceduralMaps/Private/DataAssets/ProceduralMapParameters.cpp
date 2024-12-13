// Copyright Face Software. All Rights Reserved.


#include "DataAssets/ProceduralMapParameters.h"


TSubclassOf<ALimenProceduralMapManager> UProceduralMapParameters::GetManagerClass() const
{
	PURE_VIRTUAL(UProceduralMapParameters::GetMapManagerClass)
	return nullptr;
}

TSubclassOf<ULimenMapAlgorithm> UProceduralMapParameters::GetGenerationAlgorithm() const
{
	PURE_VIRTUAL(UProceduralMapParameters::GetMapBuildAlgorithm)
	return nullptr;
}

bool UProceduralMapParameters::ValidateParameters() const
{
	PURE_VIRTUAL(UProceduralMapParameters::ValidateParameters)
	return false;
}
