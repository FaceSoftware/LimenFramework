// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "LimenCoreObjectInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class ULimenCoreObjectInterface : public UInterface
{
	GENERATED_BODY()
};


DECLARE_MULTICAST_DELEGATE_OneParam(FObjectInitializeDelegate, UObject* /* Object */);

/**
 * 
 */
class LIMENCORE_API ILimenCoreObjectInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

protected:
	virtual void Initialize() = 0;

private:
};
