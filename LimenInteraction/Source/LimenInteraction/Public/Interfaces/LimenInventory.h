// Copyright FaceSoftware. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "LimenInventory.generated.h"

class ILimenInventoryItem;
// This class does not need to be modified.
UINTERFACE()
class ULimenInventory : public UInterface
{
	GENERATED_BODY()
};



DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInventoryItemUpdate, const TScriptInterface<ILimenInventoryItem>&, Item);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInventoryUpdate, const TScriptInterface<ILimenInventory>&, Inventory);

/**
 * 
 */
class LIMENINTERACTION_API ILimenInventory
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual FInventoryUpdate& GetOnInventoryUpdated() = 0;
	virtual FInventoryUpdate& GetOnInventoryRefreshed() = 0;
	virtual FInventoryItemUpdate& GetOnItemFailedToAdd() = 0;
	virtual FInventoryItemUpdate& GetOnItemAdded() = 0;
	virtual FInventoryItemUpdate& GetOnItemRemoved() = 0;
	virtual FInventoryItemUpdate& GetOnItemUpdated() = 0;
};
