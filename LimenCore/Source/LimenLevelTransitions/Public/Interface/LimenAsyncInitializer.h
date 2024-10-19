// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "LimenAsyncInitializer.generated.h"

// This class does not need to be modified.
UINTERFACE()
class ULimenAsyncInitializer : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class LIMENLEVELTRANSITIONS_API ILimenAsyncInitializer
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, Category="Limen|Level Transitiona")
	bool IsWorking() const;
	virtual bool IsWorking_Implementation() const = 0;
};
