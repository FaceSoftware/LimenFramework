// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "LimenEndTileInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class PROCEDURALMAPS_API ULimenEndTileInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROCEDURALMAPS_API ILimenEndTileInterface
{
	GENERATED_BODY()
	
	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	FTransform GetSpawnTransform() const;
};
