// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Items/LimenItemBase.h"
#include "LimenPhysicalItem.generated.h"

class ULimenUpgradeManager;


/**
 * Base class for items that have a physical representation after being picked up, for example, a weapon.
 * Not suitable for items like health potions, notes, etc.
 */
UCLASS(Abstract, Blueprintable, BlueprintType)
class LIMENINTERACTION_API ALimenPhysicalItem : public ALimenItemBase
{
	GENERATED_BODY()

public:
	explicit ALimenPhysicalItem(const FObjectInitializer& InObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable)
	const FName& GetAttachmentSocket() const;
	
protected:
	UPROPERTY(EditAnywhere, Category="Limen|Animation")
	FName AttachSocket;

private:	
	uint32 CurrentUpgrade;
	bool bIsHolstered;
};
