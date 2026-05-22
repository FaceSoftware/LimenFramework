// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "LimenToggle.generated.h"

class AController;
// This class does not need to be modified.
UINTERFACE(BlueprintType, Blueprintable)
class LIMENCORE_API ULimenToggle : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class LIMENCORE_API ILimenToggle
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void TurnOn(AController* Controller, APawn* Instigator);
	
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void TurnOff(AController* Controller, APawn* Instigator);
};
