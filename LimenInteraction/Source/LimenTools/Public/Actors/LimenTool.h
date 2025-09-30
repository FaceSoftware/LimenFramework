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
	FBatteryChanged OnBatteryChanged;
	
	UPROPERTY(BlueprintAssignable)
	FToolActivationDelegate OnToolActiveStateChanged;
	
	ALimenTool();
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	bool ActivateTool();
	UFUNCTION(BlueprintCallable)
	void DeactivateTool();

	UFUNCTION(BlueprintCallable)
	bool Recharge(ALimenBattery* NewBattery);
	UFUNCTION(BlueprintCallable)
	float GetCurrentBatteryPercentage() const;

	virtual bool CanActivate() const;
	UFUNCTION(BlueprintCallable, Category="Limen|Tools")
	bool IsActive() const { return bIsActive; }

	virtual void PostDataLoaded() override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Limen")
	TObjectPtr<ULimenAbilityComponent> AbilityComponent;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Limen")
	bool bStartActive;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Limen")
	float DrainPerSecond;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Limen")
	float BatteryRechargeDelaySeconds;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Limen")
	TSubclassOf<ALimenBattery> CompatibleBatteryClass;

	/**
	 * @brief Called after a request to activate this tool is made, should be overriden to implement the specific tool's logic.
	 * Eg. Make light visible for a flashlight.
	 */
	virtual void ToolActivated();
	/**
	 * @brief Called after a request to deactivate this tool is made, should be overriden to implement the specific tool's logic.
	 * Eg. Make light invisible for a flashlight.
	 */
	virtual void ToolDeactivated();
	
	/**
	 * @brief Called after a request to deactivate this tool is made.
	 */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, DisplayName="On Tool Activated")
	void BP_OnToolActivated();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, DisplayName="On Tool Deactivated")
	void BP_OnToolDeactivated();
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnBatteryRecharge(const float RechargeTime);

private:
	UPROPERTY()
	TWeakObjectPtr<ALimenBattery> LastUsedBattery;
	UPROPERTY()
	TWeakObjectPtr<ULimenBatteryAttribute> BatteryAttribute;
	UPROPERTY(SaveGame)
	double CurrentBatteryPercentage;
	
	FTimerHandle RechargeTimer;
	bool bIsRecharging;
	bool bIsActive;
	bool bWasActiveBeforeRecharge;
	
	void RechargeFinished();

	UFUNCTION()
	void CurrentBatteryCapacityChanged(ULimenAttributeBase* Attribute, const float NewValue);
	UFUNCTION()
	void BatteryEmpty(ULimenAttributeBase* Attribute, const float NewValue);
};
