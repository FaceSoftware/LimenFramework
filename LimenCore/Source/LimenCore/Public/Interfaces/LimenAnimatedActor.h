// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "LimenAnimatedActor.generated.h"

// This class does not need to be modified.
UINTERFACE()
class ULimenAnimatedActor : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class LIMENCORE_API ILimenAnimatedActor
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void PlayAnimation();
};
