// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LimenSavesHandler.h"
#include "LimenItemSavesHandler.generated.h"

class ALimenWeapon;
class ALimenTool;
struct FActorSaveData;
/**
 * 
 */
UCLASS()
class LIMENGAMESAVES_API ULimenItemSavesHandler : public ULimenSavesHandler
{
	GENERATED_BODY()

public:
	virtual bool SaveDataFrom(UWorld* World) override;
	virtual bool LoadDataTo(UWorld* World) override;

private:
	UPROPERTY(SaveGame)
	TArray<FActorSaveData> ItemsSaveData;

	UPROPERTY(SaveGame)
	TSoftClassPtr<ALimenTool> HoldingToolClass;
	UPROPERTY(SaveGame)
	TSoftClassPtr<ALimenWeapon> HoldingWeaponClass;
};
