// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LimenStorage/Public/LimenStorageItem.h"
#include "LimenAttributeBase.generated.h"


class ULimenAbilityComponent;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAttributeUpdate, const float, NewValue);

/**
 * This class behaves as a meter,
 * normally used as health or stamina for a player.
 */
UCLASS(Abstract)
class LIMENABILITYSYSTEM_API ULimenAttributeBase : public ULimenStorageItem, public FTickableGameObject
{
	GENERATED_BODY()

public:
	/**
	 * @brief Delegate fired when the attribute is empty.
	 */
	UPROPERTY(BlueprintAssignable)
	FAttributeUpdate OnAttributeEmpty;

	/**
	 * @brief Delegate fired when the attribute is full.
	 */
	UPROPERTY(BlueprintAssignable)
	FAttributeUpdate OnAttributeFull;

	/**
	 * @brief Delegate fired when the attribute is updated.
	 * This also fires when the attribute is full or empty.
	 */
	UPROPERTY(BlueprintAssignable)
	FAttributeUpdate OnAttributeChanged;
	
	
public:
	ULimenAttributeBase();
	
	/**
	 * @brief Used for initialization for pawn attributes
	 * @param InOwner The pawn that owns this attribute
	 */
	virtual void Initialize(AActor* InOwner);
	/**
	 * @brief Used for initialization outside of the player's attribute system.
	 */
	virtual void Initialize();
	virtual bool IsInitialized() const;
	
	/// FTickableGameObject Interface Start
	virtual void Tick(float DeltaTime) override;
	virtual ETickableTickType GetTickableTickType() const override;
	virtual bool IsTickable() const override;
	virtual bool IsAllowedToTick() const override;
	virtual TStatId GetStatId() const override;
	virtual bool IsTickableWhenPaused() const override;
	virtual bool IsTickableInEditor() const override;
	virtual UWorld* GetTickableGameObjectWorld() const override;
	/// FTickableGameObject Interface End 

	/**
	 * @brief Sets the attribute recharge rate.
	 * @param Value The new recharge rate per second.
	 */
	UFUNCTION(BlueprintCallable, Category="Limen|Attributes")
	void SetRechargeRate(const float Value);
	
	/**
	 * @brief Sets the attribute to a new value.
	 * @param Value The new value of the attribute.
	 */
	UFUNCTION(BlueprintCallable, Category="Limen|Attributes")
	void SetValue(const float Value);

	/**
	 * @brief Modifies the current value by a specified amount.
	 * Negative numbers decrement the current value.
	 * @param Value The amount to change the value by.
	 */
	UFUNCTION(BlueprintCallable, Category="Limen|Attributes")
	void ModifyValueBy(const float Value);

	/**
	 * @brief Similar to ModifyValueBy but, if it exceeds the attribute's max/min limits, it returns the amount
	 * that exceeds the max/min thresholds (egg. If you remove 5 liters from a full 3 liter container,
	 * it would return 2 liters).
	 * @param Value The amount to change the value by.
	 * @return The overflowed amount.
	 */
	UFUNCTION(BlueprintCallable, Category="Limen|Attributes")
	float SafeModifyValueBy(const float Value);

	/**
	 * @brief Modifies the current value by a specific percentage (negative percentages would decrement the attribute).
	 * @param Percent The percentage to modify by.
	 */
	UFUNCTION(BlueprintCallable, Category="Limen|Attributes")
	void ModifyValuePercentageBy(const float Percent);

	/**
	 * @brief Changes the maximum value of this attribute.
	 * @param Value The new maximum value.
	 */
	UFUNCTION(BlueprintCallable, Category="Limen|Attributes")
	void ModifyMaxValueBy(const float Value);

	/**
	 * @brief Similar to ModifyMaxValueBy but uses percentage.
	 * @param Percent The percentage to modify by.
	 */
	UFUNCTION(BlueprintCallable, Category="Limen|Attributes")
	void ModifyMaxValuePercentageBy(const float Percent);

	/**
	 * @brief Empties the attribute value.
	 */
	UFUNCTION(BlueprintCallable, Category="Limen|Attributes")
	void Empty();

	
	/**
	 * @brief Sets the attribute value to the maximum.
	 */
	UFUNCTION(BlueprintCallable, Category="Limen|Attributes")
	void Full();

	/**
	 * @brief Gets the current max value.
	 * @return The current max value.
	 */
	UFUNCTION(BlueprintCallable, Category="Limen|Attributes")
	float GetMaxValue() const;
	/**
	 * @brief Gets the current minimum value.
	 * @return The current minimum value.
	 */
	UFUNCTION(BlueprintCallable, Category="Limen|Attributes")
	float GetMinValue() const;
	/**
	 * @brief Getter for the current value of this attribute.
	 * @return The current value of the attribute.
	 */
	UFUNCTION(BlueprintCallable, Category="Limen|Attributes")
	float GetCurrentValue() const;
	/**
	 * @brief Gets the current value of this attribute but in percentage.
	 * @return The percentage of the current value.
	 */
	UFUNCTION(BlueprintCallable, Category="Limen|Attributes")
	float GetCurrentValueAsPercentage() const;

	/**
	 * @brief Checks if thi attribute is empty.
	 * @return True if empty, false if not.
	 */
	UFUNCTION(BlueprintCallable)
	bool IsEmpty() const;
	
	UFUNCTION(BlueprintCallable)
	bool IsFull() const;

	/**
	 * @brief Gets how much is needed to reach the maximum value.
	 * @return How many units are needed to reach the maximum value.
	 */
	UFUNCTION(BlueprintCallable)
	float GetValueUntilMax() const { return MaxValue - CurrentValue; }

	/**
	 * @brief Checks if the current value does not exceed it's limits.
	 * @return True if everything is okay, false if not.
	 */
	bool StatsMakeSense() const;

	void SetCurrentValueAsMax();
	void SetCurrentValueAsMin();

	/**
	 * @brief Sets the current value ignoring restrictions.
	 * @param Value The new value.
	 */
	void SetCurrentValueAs(const float Value);

	ULimenAbilityComponent* GetOwnerAbilityComponent() const;	
	AActor* GetOwner() const;
	/**
	 * @brief Freezing an attribute essentially freezing the value not allowing any modifications
	 * @param bShouldFreeze Whether or not to freeze the attribute
	 */
	void FreezeAttribute(const bool bShouldFreeze);
	bool IsFrozen() const;

protected:
	/**
	 * @brief The amount to recharge every second.
	 */
	UPROPERTY(EditAnywhere, Category="Limen")
	float RechargeRate;
	UPROPERTY(EditAnywhere)
	float MaxValue;
	UPROPERTY(EditAnywhere)
	float InitialValue;

	virtual void AttributeEmpty() {};
	virtual void AttributeFull() {};
	virtual void AttributeUpdated() {};
	
private:
	
	const float MinValue = 0.f;

	UPROPERTY(SaveGame)
	float CurrentValue;

	UPROPERTY()
	TObjectPtr<AActor> Owner;
	UPROPERTY()
	TObjectPtr<ULimenAbilityComponent> AbilityComponent;
	bool bIsInitialized;
	bool bIsFrozen;
};
