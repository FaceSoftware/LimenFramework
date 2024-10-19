// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Items/LimenPhysicalItem.h"
#include "LimenWeapon.generated.h"


class ULimenCameraTiltComponent;
class USpringArmComponent;
class UCameraComponent;
class UAIPerceptionSystem;
class ULimenAttributeComponent;
class ULimenInventoryComponent;
class ALimenAmmo;
class ULimenWeaponDataAsset;
class ALimenWeapon;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAmmoUpdateDelegate, const int, CurrentAmmo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FWeaponReload, ALimenWeapon*, Weapon, const float, ReloadTime);

UCLASS(Blueprintable, BlueprintType)
class LIMENWEAPONS_API ALimenWeapon : public ALimenPhysicalItem
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FAmmoUpdateDelegate OnAmmoUpdated;
	UPROPERTY(BlueprintAssignable)
	FWeaponReload OnWeaponReload;
	
	explicit ALimenWeapon(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	void StartFiring();
	void StopFiring();
	void StartReloading();
	void StopReloading();

	bool IsFiring() const;
	float GetSecondsPerShot() const;
	float GetReloadTime() const;
	bool IsReloading() const;
	virtual bool CanReload() const;
	virtual bool CanFire() const;
	float GetBaseDamage() const;
	const TSubclassOf<ALimenAmmo>& GetCompatibleAmmo() const;
	UFUNCTION(BlueprintCallable)
	int32 GetCurrentAmmo() const;
	int32 GetAmmoCountUntilFull() const;
		
protected:	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Limen", meta=(ClampMin="1"))
	float BaseDamage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Limen", meta=(ClampMin="1"))
	int32 RoundsPerSecond;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Limen", meta=(ClampMin="1"))
	int32 MagazineCapacity;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Limen", meta=(ClampMin="0"))
	float ReloadTimeInSeconds;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Limen")
	bool bIsAutomatic;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Limen")
	TSubclassOf<ALimenAmmo> CompatibleAmmo;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Limen", meta=(ClampMin="1"))
	float WeaponRange;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Limen", meta=(ClampMin="0"))
	float ImpactDamageFalloffMultiplier;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Limen", meta=(ClampMin="0"))
	float FireSoundRange;
	
	void DecrementAmmo(const int Value = 1);
	void SetCurrentAmmo(const int32 NewValue);
	
	virtual void OnReloadStart(const float ReloadTimeSeconds);
	virtual void WeaponFired();
	virtual void WeaponFiredWithoutAmmo();

	void Fire();
	virtual void FireMethod();
	UFUNCTION()
	virtual void Reload();

	UFUNCTION()
	virtual void OnRep_CurrentAmmo();
		
private:
	UPROPERTY(ReplicatedUsing="OnRep_CurrentAmmo", EditDefaultsOnly, Category="Limen", meta=(ClampMin="0"))
	int32 CurrentAmmo;

	UPROPERTY(Replicated)
	bool bIsReloading;
	float TimeBetweenShots;
	bool bIsHoldingTrigger;
	bool bIsFireRateCooldownOver;
	UPROPERTY(Replicated)
	bool bIsFiring;
	FTimerHandle FireRateTimer;
	FTimerHandle CooldownTimer;

	FTimerHandle ReloadTimer;
	
	bool bIsNextShotReady;

	void HandleWeaponCooldown();

	void SetNextShotReady();
	
	void StartReloadTimer();
	void StopReloadTimer();

	UPROPERTY()
	TObjectPtr<UAIPerceptionSystem> AIPerceptionSystem;
};
