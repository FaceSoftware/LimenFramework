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

public:
	void BindPawn(APawn* Pawn);

protected:
	/**
	 * @brief Event fired when a character is associated with this HUD.
	 * BindCharacter function should be called by the PlayerController
	 * to associate the controlled pawn. After that this event is fired.
	 * @param Pawn The character associated with this HUD.
	 */
	UFUNCTION(BlueprintImplementableEvent)
	void OnPawnBound(APawn* Pawn);

	UFUNCTION(BlueprintImplementableEvent)
	void WeaponChanged(ALimenWeapon* NewWeapon, ALimenWeapon* OldWeapon);

	UFUNCTION(BlueprintImplementableEvent)
	void ToolChanged(ALimenTool* NewTool, ALimenTool* OldTool);

	/**
	 * @brief Getter for the bound character.
	 * @return The character bound with this HUD.
	 */
	UFUNCTION(BlueprintCallable)
	APawn* GetBoundPawn() const;

private:
	UPROPERTY()
	TObjectPtr<APawn> BoundPawn;
};
