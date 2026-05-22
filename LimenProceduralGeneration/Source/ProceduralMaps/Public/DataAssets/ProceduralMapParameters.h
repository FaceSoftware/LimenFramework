// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ProceduralMapParameters.generated.h"

class ULimenMapAlgorithm;
class ALimenProceduralMapManager;

/**
 * 
 */
UCLASS()
class PROCEDURALMAPS_API UProceduralMapParameters : public UDataAsset
{
	GENERATED_BODY()

public:
	virtual bool ValidateParameters() const;
	virtual TSubclassOf<ALimenProceduralMapManager> GetManagerClass() const;
	virtual TSubclassOf<ULimenMapAlgorithm> GetGenerationAlgorithm() const;

protected:
};
