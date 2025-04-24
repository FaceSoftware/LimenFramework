// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Items/LimenPhysicalItem.h"
#include "Perception/AISense_Hearing.h"
#include "LimenWeapon.generated.h"


class ULimenWeaponFireMethod;
class ULimenDamageType;
class UCameraShakeBase;
class ULimenCameraTiltComponent;
class USpringArmComponent;
class UCameraComponent;
class UAIPerceptionSystem;
class ULimenAttributeComponent;
class ULimenInventoryComponent;
class ALimenAmmo;
class ULimenWeaponDataAsset;
class ALimenWeapon;

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	None,
	Active,
	Reload
};

USTRUCT(BlueprintType)
struct FAmmoData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	int CurrentAmmo = 0;
	
	UPROPERTY(BlueprintReadOnly)
	int SpareAmmo = 0;
};	

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAmmoUpdateDelegate, const int, CurrentAmmo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FWeaponStateDelegate, ALimenWeapon*, Weapon, const EWeaponState, CurrentState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FWeaponReload, ALimenWeapon*, Weapon, const float, ReloadTime);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWeaponFireDelegate, ALimenWeapon*, Weapon);


/**
 * ALimenWeapon is a class representing a weapon in the LimenWeapons system.
 * It inherits from ALimenPhysicalItem and adds functionality specific to weapons,
 * including firing, reloading, and handling ammo updates.
 * This class supports both single-shot and automatic firing modes.
 */
UCLASS(Blueprintable, BlueprintType)
class LIMENWEAPONS_API ALimenWeapon : public ALimenPhysicalItem
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FAmmoUpdateDelegate OnAmmoUpdated;
	UPROPERTY(BlueprintAssignable)
	FWeaponStateDelegate OnWeaponStateUpdated;
	UPROPERTY(BlueprintAssignable)
	FWeaponReload OnWeaponReload;
	UPROPERTY(BlueprintAssignable)
	FWeaponFireDelegate OnWeaponFired;
	UPROPERTY(BlueprintAssignable)
	FWeaponFireDelegate OnWeaponCooldownOver;
	
	ALimenWeapon();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void Drop(AController* InController, APawn* InPawn) override;
	virtual void PickUp(AController* InController, APawn* InPawn) override;
	
	void StartFiring();
	void StopFiring();
	bool StartReloading(ULimenInventoryComponent* PlayerInventory);
	void StopReloading();

	UFUNCTION(BlueprintCallable)
	bool IsFiring() const;
	UFUNCTION(BlueprintCallable)
	float GetSecondsPerShot() const;
	UFUNCTION(BlueprintCallable)
	float GetReloadTime() const;
	UFUNCTION(BlueprintCallable)
	bool IsReloading() const;
	UFUNCTION(BlueprintCallable)
	bool CanReload(const ULimenInventoryComponent* PlayerInventory) const;
	UFUNCTION(BlueprintCallable)
	bool CanFire() const;
	UFUNCTION(BlueprintCallable)
	float GetBaseDamage() const;
	UFUNCTION(BlueprintCallable)
	float GetWeaponRange() const;
	UFUNCTION(BlueprintCallable)
	TSubclassOf<ULimenDamageType> GetDamageType() const;
	UFUNCTION(BlueprintCallable)
	float GetImpactDamageFalloffMultiplier() const;
	UFUNCTION(BlueprintCallable)
	float GetAINoiseEventLoudness() const;
	UFUNCTION(BlueprintCallable)
	float GetFireSoundRange() const;
	

	const TSubclassOf<ALimenAmmo>& GetCompatibleAmmo() const;

	UFUNCTION(BlueprintCallable)
	uint8 GetCurrentAmmo() const;

	uint8 GetAmmoCountUntilFull() const;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, Category="Limen|Weapons")
	void BP_WeaponReload();
		
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Parameters")
	TSoftClassPtr<ULimenWeaponFireMethod> FireMethodClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Parameters")
	TSubclassOf<ULimenDamageType> DamageType;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Parameters", meta=(ClampMin="1"))
	float BaseDamage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Parameters", meta=(ClampMin="1"))
	int32 RoundsPerSecond;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Parameters", meta=(ClampMin="1"))
	int32 MagazineCapacity;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Parameters", meta=(ClampMin="0"))
	float ReloadTimeInSeconds;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Parameters")
	bool bIsAutomatic;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Parameters")
	TSubclassOf<ALimenAmmo> CompatibleAmmo;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Parameters", meta=(ClampMin="0"))
	int32 CurrentAmmo;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Parameters", meta=(ClampMin="1"))
	float WeaponRange;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Parameters", meta=(ClampMin="0"))
	float ImpactDamageFalloffMultiplier;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Parameters", meta=(ClampMin="0"))
	float FireSoundRange;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Parameters", meta=(ClampMin="0"))
	float AINoiseEventLoudness;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Parameters")
	TSubclassOf<UCameraShakeBase> RecoilCameraShakeClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Parameters", meta=(ClampMin="0"))
	float RecoilCameraShakeScale;
	
	void DecrementAmmo(const int Value = 1);
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, Category="Limen|Weapons", DisplayName=ReloadStart)
	void BP_ReloadStart(const float ReloadTimeSeconds);
	void ReloadStart(const float ReloadTimeSeconds);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, Category="Limen|Weapons", DisplayName=WeaponFired)
	void BP_WeaponFired();
	virtual void WeaponFired();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, Category="Limen|Weapons", DisplayName=WeaponFiredWithoutAmmo)
	void BP_WeaponFiredWithoutAmmo();
	virtual void WeaponFiredWithoutAmmo();

	void Fire();
	
private:
	UPROPERTY(Replicated)
	float TimeBetweenShots;
	UPROPERTY(Replicated)
	bool bIsHoldingTrigger;
	UPROPERTY(Replicated)
	bool bIsFireRateCooldownOver;
	UPROPERTY(Replicated)
	bool bIsFiring;
	FTimerHandle FireRateTimer;
	FTimerHandle CooldownTimer;
	FTimerHandle ReloadTimer;
	UPROPERTY(Replicated)
	bool bIsReloading;
	UPROPERTY(Replicated)
	bool bIsNextShotReady;
	UPROPERTY(Replicated)
	EWeaponState CurrentWeaponState;
	TStrongObjectPtr<ULimenWeaponFireMethod> FireMethodObject;

	void HandleWeaponCooldown();
	void SetNextShotReady();
	UFUNCTION()
	void Reload(ULimenInventoryComponent* PlayerInventory);
	void StartReloadTimer(ULimenInventoryComponent* PlayerInventory);
	void StopReloadTimer();
	
	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_WeaponFired();
};
