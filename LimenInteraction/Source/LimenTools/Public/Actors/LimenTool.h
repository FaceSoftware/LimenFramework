// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Items/LimenPhysicalItem.h"
#include "LimenTool.generated.h"


class ULimenAttributeBase;
class ULimenAbilityComponent;
class ULimenBatteryAttribute;
class FThreadedFunction;
class ALimenBattery;
struct FBatteryData;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBatteryChanged, const double, BatteryPercentage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FToolActivationDelegate, const bool, bIsActive);

UCLASS(Blueprintable, BlueprintType)
class LIMENTOOLS_API ALimenTool : public ALimenPhysicalItem
{
	GENERATED_BODY()

public:	
	UPROPERTY(BlueprintAssignable)
	FToolActivationDelegate OnToolActiveStateChanged;
	
	ALimenTool();
	virtual void BeginPlay() override;

	bool ActivateTool();
	void DeactivateTool();
	bool ToggleTool();

	virtual bool CanActivate() const;
	bool IsActive() const;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Limen")
	TObjectPtr<ULimenAbilityComponent> AbilityComponent;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Limen")
	bool bStartActive;

	/**
	 * @brief Called after a request to activate this tool is made, should be overriden to implement the specific tool's logic.
	 * E.g. Make light visible for a flashlight.
	 */
	virtual void ToolActivated();
	/**
	 * @brief Called after a request to deactivate this tool is made, should be overriden to implement the specific tool's logic.
	 * E.g. Make light invisible for a flashlight.
	 */
	virtual void ToolDeactivated();

private:
	UPROPERTY(SaveGame, Transient)
	bool bIsActive;
};
