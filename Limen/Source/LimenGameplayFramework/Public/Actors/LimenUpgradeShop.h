// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actors/LimenUpgradeShopBase.h"
#include "LimenUpgradeShop.generated.h"

UCLASS()
class LIMENGAMEPLAYFRAMEWORK_API ALimenUpgradeShop : public ALimenUpgradeShopBase
{
	GENERATED_BODY()

public:
	explicit ALimenUpgradeShop(const FObjectInitializer& InObjectInitializer = FObjectInitializer::Get());

protected:
	UFUNCTION(BlueprintCallable, Category="Limen|Upgrades")
	void DisplayUpgradeShop(APlayerController* PlayerController, APawn* InPawn) const;
	
private:
	
};
