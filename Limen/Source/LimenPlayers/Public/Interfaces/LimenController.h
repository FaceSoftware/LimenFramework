// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "LimenController.generated.h"

// This class does not need to be modified.
UINTERFACE()
class ULimenController : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class LIMENPLAYERS_API ILimenController
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	ILimenController();
	
	void LookAt(const FVector& Location);

private:
	
};
