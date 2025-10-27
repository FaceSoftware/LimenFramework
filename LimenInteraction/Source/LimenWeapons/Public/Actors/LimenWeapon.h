// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Items/LimenPhysicalItem.h"
#include "LimenWeapon.generated.h"


class ADecalActor;
class ULimenWeaponFireMethod;
class ULimenDamageType;
class UCameraShakeBase;
class ULimenCameraComponent;
class USpringArmComponent;
class UCameraComponent;
class UAIPerceptionSystem;
class ULimenAttributeComponent;
class ULimenInventoryComponent;
class ALimenAmmo;
class ULimenWeaponDataAsset;
class ALimenWeapon;

USTRUCT(BlueprintType)
struct FAmmoData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	int CurrentAmmo = 0;
	
	UPROPERTY(BlueprintReadOnly)
	int SpareAmmo = 0;
};

UENUM(BlueprintType)
enum class EInfiniteAmmoType : uint8
{
	Disabled,
	InfiniteBullets,
	InfiniteMagazines,
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAmmoUpdateDelegate, const int, CurrentAmmo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FWeaponStateDelegate, ALimenWeapon*, Weapon, const bool, bIsDropped);
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
	FWeaponReload OnWeaponReloadCanceled;
	UPROPERTY(BlueprintAssignable)
	FWeaponFireDelegate OnWeaponFired;
	UPROPERTY(BlueprintAssignable)
	FWeaponFireDelegate OnWeaponCooldownOver;
	
	explicit ALimenWeapon(const FObjectInitializer& InObjectInitializer = FObjectInitializer::Get());
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;;
	virtual void Tick(float DeltaSeconds) override;

	virtual void Drop(AController* InController, APawn* InPawn) override;
	virtual void PickUp(AController* InController, APawn* InPawn) override;
	
	void StartFiring();
	void StopFiring();
	void StartReloading();
	void StopReloading();

	UFUNCTION(BlueprintCallable)
	bool IsFiring() const;
	UFUNCTION(BlueprintCallable)
	bool IsReloading() const;
	UFUNCTION(BlueprintCallable)
	virtual bool CanReload() const;
	UFUNCTION(BlueprintCallable)
	virtual bool CanFire() const;
	UFUNCTION(BlueprintCallable)
	TSubclassOf<ALimenAmmo> GetCompatibleAmmo() const;
	UFUNCTION(BlueprintCallable)
	int32 GetCurrentAmmo() const;
	UFUNCTION(BlueprintCallable)
	int32 GetAmmoCountUntilFull() const;

	UFUNCTION(BlueprintCallable)
	float GetSecondsPerShot() const;
	UFUNCTION(BlueprintCallable)
	void SetSecondsPerShot(const float NewValue);

	UFUNCTION(BlueprintCallable)
	float GetReloadTime() const;
	UFUNCTION(BlueprintCallable)
	void SetReloadTime(const float NewValue);

	UFUNCTION(BlueprintCallable)
	float GetBaseDamage() const;
	UFUNCTION(BlueprintCallable)
	void SetBaseDamage(const float NewValue);

	UFUNCTION(BlueprintCallable)
	float GetWeaponRange() const;
	UFUNCTION(BlueprintCallable)
	void SetWeaponRange(const float NewValue);

	UFUNCTION(BlueprintCallable)
	TSubclassOf<ULimenDamageType> GetDamageType() const;
	UFUNCTION(BlueprintCallable)
	void SetDamageType(const TSubclassOf<ULimenDamageType>& NewValue);

	UFUNCTION(BlueprintCallable)
	float GetImpactDamageFalloffMultiplier() const;
	UFUNCTION(BlueprintCallable)
	void SetImpactDamageFalloffMultiplier(const float NewValue);

	UFUNCTION(BlueprintCallable)
	float GetAINoiseEventLoudness() const;
	UFUNCTION(BlueprintCallable)
	void SetAINoiseEventLoudness(const float NewValue);

	UFUNCTION(BlueprintCallable)
	float GetFireSoundRange() const;
	UFUNCTION(BlueprintCallable)
	void SetFireSoundRange(const float NewValue);

	UFUNCTION(BlueprintCallable)
	EInfiniteAmmoType GetInfiniteAmmoType() const;
	UFUNCTION(BlueprintCallable)
	void SetInfiniteAmmoType(const EInfiniteAmmoType NewValue);

	UFUNCTION(BlueprintCallable)
	virtual FVector GetFiringLocation() const;
		
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Parameters")
	TSoftClassPtr<ULimenWeaponFireMethod> FireMethodClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Parameters")
	TSubclassOf<ULimenDamageType> DamageType;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Parameters", meta=(ClampMin="0"), Replicated)
	float BaseDamage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Parameters", meta=(ClampMin="0"), Replicated)
	float SecondsPerShot;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Parameters", meta=(ClampMin="1"), Replicated)
	int32 MagazineCapacity;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Parameters", meta=(ClampMin="0"), Replicated)
	float ReloadTimeInSeconds;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Parameters", Replicated)
	bool bIsAutomatic;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Parameters")
	TSubclassOf<ALimenAmmo> CompatibleAmmo;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Parameters", meta=(ClampMin="0"))
	int32 InitialAmmo;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Parameters", meta=(ClampMin="0"), Replicated)
	float WeaponRange;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Parameters", meta=(ClampMin="0"), Replicated)
	float ImpactDamageFalloffMultiplier;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Parameters", meta=(ClampMin="0"), Replicated)
	float FireSoundRange;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Parameters", meta=(ClampMin="0"), Replicated)
	float AINoiseEventLoudness;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Parameters")
	TSubclassOf<UCameraShakeBase> RecoilCameraShakeClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Parameters", Replicated)
	bool bIsSilenced;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Parameters", Replicated)
	EInfiniteAmmoType InfiniteAmmoType;

	
	virtual void DecrementAmmo(const int Value = 1);

	virtual void ReloadStart();
	virtual void ReloadCancelled();
	virtual void WeaponFired();
	virtual void WeaponFiredWithoutAmmo();
	virtual void StartWeaponFire();
	virtual void StopWeaponFire();

	virtual void Fire();
	UFUNCTION()
	virtual void Reload();

	virtual void OnRep_IsDropped() override;
	UFUNCTION()
	virtual void OnRep_CurrentAmmo();
	
private:
	UPROPERTY(Replicated)
	bool bIsHoldingTrigger;
	UPROPERTY(Replicated)
	bool bIsFireRateCooldownOver;
	UPROPERTY(Replicated)
	bool bIsFiring;
	UPROPERTY(Replicated)
	bool bIsReloading;
	UPROPERTY(Replicated)
	bool bIsNextShotReady;
	UPROPERTY(ReplicatedUsing=OnRep_CurrentAmmo, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	int32 CurrentAmmo;

	FTimerHandle CooldownTimer;
	FTimerHandle ReloadTimer;

	TWeakObjectPtr<UCameraShakeBase> RecoilCameraShakePtr;
	TStrongObjectPtr<ULimenWeaponFireMethod> FireMethodObject;

	int32 ShotsInARow;
	double FireAccumulator;
	uint64 SimulatingShotsCount;

	void HandleWeaponCooldown();
	void SetNextShotReady();
	void StartReloadTimer();
	void StopReloadTimer();
	void ReloadInternal();
	
	virtual void StartFireCameraShake();
	virtual void StopFireCameraShake();

	UFUNCTION(Server, Reliable)
	void Server_SetFireState(const bool bEnabled);
	UFUNCTION(Server, Reliable)
	void Server_SetReloadState(const bool bEnabled);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ReloadStart();
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ReloadCanceled();
	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_StartWeaponFire();
	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_StopWeaponFire();
	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_WeaponFired();

};
