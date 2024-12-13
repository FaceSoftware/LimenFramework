// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LimenSavesHandler.h"
#include "LimenObjectiveSavesHandler.generated.h"


struct FActorSaveData;
/**
 * 
 */
UCLASS()
class LIMENGAMESAVES_API ULimenObjectiveSavesHandler : public ULimenSavesHandler
{
	GENERATED_BODY()
	
public:	
	virtual void SaveDataFrom(UWorld* World) override;
	virtual void LoadDataTo(UWorld* World) override;

private:
	UPROPERTY(SaveGame)
	TArray<FActorSaveData> ObjectivesSaveData;
};
