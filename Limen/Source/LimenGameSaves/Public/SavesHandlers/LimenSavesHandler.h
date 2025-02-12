// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/LimenSaveObjectInterface.h"
#include "LimenSavesHandler.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class LIMENGAMESAVES_API ULimenSavesHandler : public UObject, public ILimenSaveObjectInterface
{
	GENERATED_BODY()
	
public:
	// ILimenSaveObjectInterface - Start
	virtual bool ShouldSaveData() const override final;
	virtual bool ShouldLoadData() const override final;
	virtual void DataSaved() override final;
	virtual void DataLoaded() override final;
	// ILimenSaveObjectInterface - End
	
	virtual bool SaveDataFrom(UWorld* World) { return true; }
	virtual bool LoadDataTo(UWorld* World) { return true; }

};
