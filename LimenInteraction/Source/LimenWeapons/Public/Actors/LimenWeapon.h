// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Items/LimenPhysicalItem.h"
#include "Perception/AISense_Hearing.h"
#include "LimenWeapon.generated.h"


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
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FWeaponReload, ALimenWeapon*, Weapon, const double, ReloadTime);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWeaponFireDelegate, ALimenWeapon*, Weapon);


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
	virtual void BeginPlay() override;
	void Drop();
	virtual void PickUp(AController* InController, APawn* InPawn) override;
	
	void StartFiring();
	void StopFiring();
	bool StartReloading(ULimenInventoryComponent* PlayerInventory);
	void StopReloading();

	UFUNCTION(BlueprintCallable)
	bool IsFiring() const;
	double GetSecondsPerShot() const;
	double GetReloadTime() const;
	bool IsReloading() const;
	bool CanReload(const ULimenInventoryComponent* PlayerInventory) const;
	bool CanFire() const;
	double GetBaseDamage() const;

	const TSubclassOf<ALimenAmmo>& GetCompatibleAmmo() const;

	UFUNCTION(BlueprintCallable)
	uint8 GetCurrentAmmo() const;

	uint8 GetAmmoCountUntilFull() const;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, Category="Limen|Weapons")
	void BP_WeaponReload();
		
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Parameters")
	TSubclassOf<ULimenDamageType> DamageType;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Parameters", meta=(ClampMin="1"))
	double BaseDamage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Parameters", meta=(ClampMin="1"))
	int32 RoundsPerSecond;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Parameters", meta=(ClampMin="1"))
	int32 MagazineCapacity;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Parameters", meta=(ClampMin="0"))
	double ReloadTimeInSeconds;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Parameters")
	bool bIsAutomatic;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Parameters")
	TSubclassOf<ALimenAmmo> CompatibleAmmo;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Parameters", meta=(ClampMin="0"))
	int32 CurrentAmmo;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Parameters", meta=(ClampMin="1"))
	double WeaponRange;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Parameters", meta=(ClampMin="0"))
	double ImpactDamageFalloffMultiplier;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Parameters", meta=(ClampMin="0"))
	double FireSoundRange;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Parameters", meta=(ClampMin="0"))
	float AINoiseEventLoudness;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Parameters")
	TSubclassOf<UCameraShakeBase> RecoilCameraShakeClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Parameters", meta=(ClampMin="0"))
	float RecoilCameraShakeScale;
	
	void DecrementAmmo(const int Value = 1);
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, Category="Limen|Weapons")
	void ReloadStart(const double ReloadTimeSeconds);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, Category="Limen|Weapons")
	void BP_WeaponFired();
	void WeaponFired();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, Category="Limen|Weapons")
	void WeaponFiredWithoutAmmo();

	void Fire();
	virtual void FireMethod() {}
	
private:
	double TimeBetweenShots;
	bool bIsHoldingTrigger;
	bool bIsFireRateCooldownOver;
	bool bIsFiring;
	FTimerHandle FireRateTimer;
	FTimerHandle CooldownTimer;

	FTimerHandle ReloadTimer;
	bool bIsReloading;
	
	bool bIsNextShotReady;

	void HandleWeaponCooldown();

	void SetNextShotReady();

	UFUNCTION()
	void Reload(ULimenInventoryComponent* PlayerInventory);
	void StartReloadTimer(ULimenInventoryComponent* PlayerInventory);
	void StopReloadTimer();
	
	EWeaponState CurrentWeaponState;
};
