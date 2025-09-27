// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "LimenSaveObjectInterface.generated.h"

class ULimenSaveData;
// This class does not need to be modified.
UINTERFACE()
class LIMENSAVESYSTEM_API ULimenSaveObjectInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class LIMENSAVESYSTEM_API ILimenSaveObjectInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	/**
	 * @brief Checks if this object's data should be saved.
	 * @return True if it can, false otherwise.
	 */
	virtual bool ShouldSaveData() const = 0;
	/**
	 * @brief Checks if this object can be used for loading data.
	 * @return True if it can, false otherwise.
	 */
	virtual bool ShouldLoadData() const = 0;
	/**
	 * @brief Called after a save was performed on the object.
	 */
	virtual void DataSaved() = 0;
	/**
	 * @brief Called when data has been loaded into the object.
	 */
	virtual void DataLoaded() = 0;
};
