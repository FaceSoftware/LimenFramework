// Copyright FaceSoftware. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "LimenInventoryItem.h"
#include "UObject/Interface.h"
#include "LimenSlotInventoryItem.generated.h"

// This class does not need to be modified.
UINTERFACE()
class ULimenSlotInventoryItem : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class LIMENINTERACTION_API ILimenSlotInventoryItem
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual TArray<FName> CompatibleSlots() const = 0;
	virtual FName DefaultSlot() const = 0;
};
