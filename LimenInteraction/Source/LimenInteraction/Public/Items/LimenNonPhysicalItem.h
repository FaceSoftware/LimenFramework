// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Items/LimenItemBase.h"
#include "LimenNonPhysicalItem.generated.h"

/**
 * Base class for items that do not have a physical representation after being picked up, for example, an health potion.
 * Not suitable for items like weapons.
 */
UCLASS()
class LIMENINTERACTION_API ALimenNonPhysicalItem : public ALimenItemBase
{
	GENERATED_BODY()
};
