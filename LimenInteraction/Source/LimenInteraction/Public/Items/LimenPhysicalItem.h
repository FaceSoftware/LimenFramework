// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
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
	
	virtual void SetTagState(const FGameplayTag& InTag, bool bEnable);

	UFUNCTION(BlueprintCallable)
	const FName& GetAttachmentSocketName() const;
	
protected:
	UPROPERTY(EditAnywhere, Category="Limen|Animation")
	FName AttachSocketName;
	
	virtual void TagStateChanged(const FGameplayTag& Tag, bool bState);
	virtual bool CanSetTagState(const FGameplayTag& InTag, bool bState);
	
	FGameplayTagContainer& GetItemStateTags();

private:
	FGameplayTagContainer ItemStateTags;
};
