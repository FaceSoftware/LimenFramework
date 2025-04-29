// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/LimenBaseHudWidget.h"
#include "LimenHudWidget.generated.h"

class ALimenTool;
class ALimenWeapon;
class ALimenCharacter;

/**
 * Base class for a HUD widget, typically displays the health, armor, ammo, etc.
 */
UCLASS(Abstract, Blueprintable)
class LIMENPLAYERS_API ULimenHudWidget : public ULimenBaseHudWidget
{
	GENERATED_BODY()
};
