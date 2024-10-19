// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LimenCharacterBase.h"
#include "LimenGameplayCharacter.h"
#include "Interfaces/LimenDamageable.h"
#include "LimenPlayerCharacter.generated.h"


class ULimenDynamicDepthOfFieldComponent;
class ULimenItemAction;
class ULimenCreditsComponent;
class ILimenInteractableComponent;
class ILimenUpgradable;
class ALimenPlayerState;
class ULimenCameraShakeComponent;
class ULimenStepsSoundComponent;
enum class EMovementStatus : uint8;
class ALimenObjective;
class ULimenPhysicalItemHoldComponent;
class USpringArmComponent;
class ULimenAttributeComponent;
class ULimenObjectiveComponent;
class ULimenInventoryComponent;
class ULimenInteractionComponent;
enum class EItemAction : uint8;
class ALimenItemBase;
class ALimenWeapon;
class ALimenTool;
class ALimenPhysicalItem;
class ALimenPlayerController;
class ULimenProxyInventory;
class ULimenCameraTiltComponent;

/**
 * Base class with all functionality. Except for interaction implementation, which should be implemented in a child class.
 * Example:
 * First Person Character - Line Trace Interaction method
 * Third Person Character - Proximity Interaction method
 */
UCLASS(Abstract, NotBlueprintable, BlueprintType)
class LIMENPLAYERS_API ALimenPlayerCharacter : public ALimenGameplayCharacter
{
	GENERATED_BODY()

public:
	static const FName HealthComponentName;
	static const FName InventoryComponentName;
	static const FName ObjectivesComponentName;
	static const FName SpringArmComponentName;
	static const FName PlayerCameraComponentName;
	static const FName WeaponHoldComponentName;
	static const FName ToolHoldComponentName;
	static const FName CreditsComponentName;
	static const FName DynamicDepthOfFieldComponentName;
	static const FName ProxyInventoryComponentName;
	
	explicit ALimenPlayerCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void UnPossessed() override;

	/**
	 * @brief Set this character to hold a physical item.
	 * @param Item 
	 */
	UFUNCTION(BlueprintCallable)
	void HoldPhysicalItem(ALimenPhysicalItem* Item);
	/**
	 * @brief Set this character to stop holding a physical item.
	 * @param ItemClass
	 */
	UFUNCTION(BlueprintCallable)
	void StopHoldingPhysicalItem(const TSubclassOf<ALimenPhysicalItem>& ItemClass);
	/**
	 * @brief Set this character to hold a tool.
	 * @param Tool 
	 */
	UFUNCTION(BlueprintCallable)
	void HoldTool(ALimenTool* Tool);
	void StopHoldingTool() const;
	void LoadHoldingTool(const TSubclassOf<ALimenTool>& ToolClass);
	/**
	 * @brief Set this character to hold a weapon.
	 * @param Weapon 
	 */
	UFUNCTION(BlueprintCallable)
	void HoldWeapon(ALimenWeapon* Weapon);
	void StopHoldingWeapon() const;
	void LoadHoldingWeapon(const TSubclassOf<ALimenWeapon>& WeaponClass);

	/**
	 * @brief Checks id this character is holding a weapon.
	 * @return True if it is holding a weapon, false otherwise.
	 */
	UFUNCTION(BlueprintCallable)
	bool IsHoldingWeapon() const;
	UFUNCTION(BlueprintCallable)
	bool IsReloading() const;
	UFUNCTION(BlueprintCallable)
	bool IsFiring() const;
	UFUNCTION(BlueprintCallable)
	ALimenWeapon* GetCurrentWeapon() const;
	UFUNCTION(BlueprintCallable)
	bool IsHoldingTool() const;
	UFUNCTION(BlueprintCallable)
	ALimenTool* GetCurrentTool() const;
	
	virtual void HandleItemActionRequests(ULimenItemAction* ActionRequested);
	
	UFUNCTION(BlueprintCallable, Category="Limen")
	ULimenCameraTiltComponent* GetPlayerCamera() const;
	UFUNCTION(BlueprintCallable, Category="Limen")
	ULimenInventoryComponent* GetInventoryComponent() const;
	UFUNCTION(BlueprintCallable, Category="Limen")
	ULimenObjectiveComponent* GetObjectivesComponent() const;
	UFUNCTION(BlueprintCallable, Category="Limen")
	USpringArmComponent* GetSpringArmComponent() const;
	UFUNCTION(BlueprintCallable, Category="Limen")
	ULimenCreditsComponent* GetCreditsComponent() const;
	UFUNCTION(BlueprintCallable, Category="Limen")
	ULimenPhysicalItemHoldComponent* GetToolHoldComponent() const;
	UFUNCTION(BlueprintCallable, Category="Limen")
	ULimenPhysicalItemHoldComponent* GetWeaponHoldComponent() const;

	UFUNCTION(BlueprintCallable)
	void GiveStartingItems(const TArray<TSubclassOf<ALimenItemBase>>& StartingItems);

	TArray<TScriptInterface<ILimenUpgradable>> GetUpgradables() const;

	virtual float ApplyPointDamage(AController* DamageInstigator, AActor* DamageCauser, const float DamageTaken) override;
	
protected:
	UPROPERTY(EditDefaultsOnly, Category="Limen")
	TObjectPtr<ULimenInventoryComponent> CharacterInventory;
	UPROPERTY(EditDefaultsOnly, Category="Limen")
	TObjectPtr<ULimenObjectiveComponent> CharacterObjectives;
	UPROPERTY(EditDefaultsOnly, Category="Limen")
	TObjectPtr<USpringArmComponent> SpringArm;
	UPROPERTY(EditDefaultsOnly, Category="Limen")
	TObjectPtr<ULimenCreditsComponent> PlayerCredits;
	UPROPERTY(EditDefaultsOnly, Category="Limen")
	TObjectPtr<ULimenDynamicDepthOfFieldComponent> DynamicDepthOfFieldComponent;
	UPROPERTY(EditDefaultsOnly, Category="Limen")
	TObjectPtr<ULimenProxyInventory> ProxyInventoryComponent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<ULimenCameraTiltComponent> PlayerCamera;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<ULimenPhysicalItemHoldComponent> WeaponHold;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<ULimenPhysicalItemHoldComponent> ToolHold;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Limen|Input Actions")
	TSoftObjectPtr<UInputAction> InteractInputAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Limen|Input Actions")
	TSoftObjectPtr<UInputAction> ToggleToolInputAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Limen|Input Actions")
	TSoftObjectPtr<UInputAction> FireInputAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Limen|Input Actions")
	TSoftObjectPtr<UInputAction> ReloadInputAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Limen|Input Actions")
	TSoftObjectPtr<UInputAction> CycleToolsInputAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Limen|Input Actions")
	TSoftObjectPtr<UInputAction> CycleWeaponsInputAction;
	
	void InteractInput(const FInputActionInstance& Instance);
	void FireInput(const FInputActionInstance& Instance);
	void ReloadInput(const FInputActionInstance& Instance);
	void ToggleToolInput(const FInputActionInstance& Instance);
	void CycleToolsInput(const FInputActionInstance& Instance);
	void CycleWeaponsInput(const FInputActionInstance& Instance);

	UFUNCTION()
	virtual void OnInteract(AActor* InteractableActor, const TScriptInterface<ILimenInteractableComponent>& InteractableComponent);
	UFUNCTION()
	virtual void ItemAdded(TSubclassOf<ALimenItemBase> NewItem);
	UFUNCTION()
	virtual void ItemCouldNotBeAdded(TSubclassOf<ALimenItemBase> NewItem);
	UFUNCTION()
	virtual void ItemUpdated(TSubclassOf<ALimenItemBase> NewItem);
	UFUNCTION()
	virtual void ObjectivesUpdated(ALimenObjective* UpdatedObjective);
	UFUNCTION()
	virtual void ToolChanged(ALimenPhysicalItem* Old, ALimenPhysicalItem* New);
	UFUNCTION()
	virtual void WeaponChanged(ALimenPhysicalItem* Old, ALimenPhysicalItem* New);

	virtual void AimDownSights();
	virtual void StopAimingDownSights();

	/**
	 * @brief When the character is aiming with the weapon.
	 * @param InWeapon The weapon that the player wants to aim with (currently holding)
	 * @warning Should not contain any gameplay logic for the weapon.
	 */
	UFUNCTION(BlueprintCosmetic, BlueprintImplementableEvent)
	void OnAimDownSights(ALimenWeapon* InWeapon);

	/**
	 * @brief When the character is aiming with the weapon.
	 * @param InWeapon The weapon that the player wants to aim with (currently holding)
	 * @warning Should not contain any gameplay logic for the weapon.
	 */
	UFUNCTION(BlueprintCosmetic, BlueprintImplementableEvent)
	void OnStopAimingDownSights(ALimenWeapon* InWeapon);
	
	/**
	 * @brief Usually where you would want to attach the weapon to the player's hand.
	 * @param InWeapon The weapon that the player wants to hold.
	 * @warning Should not contain any gameplay logic for the weapon.
	 */
	UFUNCTION(BlueprintCosmetic, BlueprintImplementableEvent)
	void OnCharacterHoldWeapon(ALimenWeapon* InWeapon);

	/**
	 * @brief Usually where you would want to attach the tool to the player's hand.
	 * @param InTool The tool that the player wants to hold.
	 * @warning Should not contain any gameplay logic for the tool.
	 */
	UFUNCTION(BlueprintCosmetic, BlueprintImplementableEvent)
	void OnCharacterHoldTool(ALimenTool* InTool);

	/**
	 * @brief Usually where you would want to do a reload animation.
	 * @param InWeapon The weapon that's being reloaded.
	 * @param ReloadTimeSeconds How long it takes to reload the weapon.
	 * @warning Should not contain any logic for reloading the weapon.
	 */
	UFUNCTION(BlueprintCosmetic, BlueprintImplementableEvent)
	void OnWeaponReload(ALimenWeapon* InWeapon, const double ReloadTimeSeconds);

	
	UFUNCTION(BlueprintCosmetic, BlueprintImplementableEvent)
	void OnToolChanged(ALimenPhysicalItem* Old, ALimenPhysicalItem* New);

	UFUNCTION(BlueprintCosmetic, BlueprintImplementableEvent)
	void OnWeaponChanged(ALimenPhysicalItem* Old, ALimenPhysicalItem* New);

	UFUNCTION(BlueprintCosmetic, BlueprintImplementableEvent, DisplayName="On Movement State Changed")
	void BP_OnRunningStateChanged();

private:
	UPROPERTY()
	TWeakObjectPtr<ALimenPlayerController> LimenPlayerControllerPtr;
	UPROPERTY()
	TWeakObjectPtr<ALimenPlayerState> LimenPlayerStatePtr;

	bool bWasFallingLastUpdate;
	bool bIsAimingDownSights;
};
