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
	void StartReloading(ULimenInventoryComponent* PlayerInventory);
	void StopReloading();

	UFUNCTION(BlueprintCallable)
	bool IsFiring() const;
	UFUNCTION(BlueprintCallable)
	float GetSecondsPerShot() const;
	UFUNCTION(BlueprintCallable)
	float GetFireRate() const;
	UFUNCTION(BlueprintCallable)
	void SetFireRate(float InFireRate);
	UFUNCTION(BlueprintCallable)
	float GetReloadTime() const;
	UFUNCTION(BlueprintCallable)
	bool IsReloading() const;
	UFUNCTION(BlueprintCallable)
	virtual bool CanReload(const ULimenInventoryComponent* PlayerInventory) const;
	UFUNCTION(BlueprintCallable)
	virtual bool CanFire() const;
	UFUNCTION(BlueprintCallable)
	float GetBaseDamage() const;
	UFUNCTION(BlueprintCallable)
	void SetBaseDamage(const float InNewDamage);
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
	UFUNCTION(BlueprintCallable)
	EInfiniteAmmoType GetInfiniteAmmoType() const;

	const TSubclassOf<ALimenAmmo>& GetCompatibleAmmo() const;

	UFUNCTION(BlueprintCallable)
	uint8 GetCurrentAmmo() const;

	uint8 GetAmmoCountUntilFull() const;

	UFUNCTION(BlueprintNativeEvent)
	FVector GetFiringLocation() const;
	virtual FVector GetFiringLocation_Implementation() const;
		
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Parameters")
	TSoftClassPtr<ULimenWeaponFireMethod> FireMethodClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Parameters")
	TSubclassOf<ULimenDamageType> DamageType;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Parameters", meta=(ClampMin="1"), Replicated)
	float BaseDamage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Parameters", meta=(ClampMin="1"), Replicated)
	float RoundsPerSecond;
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
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Parameters", meta=(ClampMin="1"), Replicated)
	float WeaponRange;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Parameters", meta=(ClampMin="0"), Replicated)
	float ImpactDamageFalloffMultiplier;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Parameters", meta=(ClampMin="0"), Replicated)
	float FireSoundRange;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Parameters", meta=(ClampMin="0"), Replicated)
	float AINoiseEventLoudness;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Parameters")
	TSubclassOf<UCameraShakeBase> RecoilCameraShakeClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Parameters", meta=(ClampMin="0"), Replicated)
	bool bIsSilenced;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Parameters", meta=(ClampMin="0"), Replicated)
	EInfiniteAmmoType InfiniteAmmoType;

	virtual void OnRep_IsDropped() override;
	
	virtual void DecrementAmmo(const int Value = 1);
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, Category="Limen|Weapons", DisplayName=ReloadStart)
	void BP_ReloadStart(const float ReloadTimeSeconds);
	virtual void ReloadStart(const float ReloadTimeSeconds);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, Category="Limen|Weapons", DisplayName=ReloadStart)
	void BP_ReloadCancelled(const float ReloadTimeSeconds);
	virtual void ReloadCancelled(const float ReloadTimeSeconds);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, Category="Limen|Weapons", DisplayName=WeaponFired)
	void BP_WeaponFired();
	virtual void WeaponFired();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, Category="Limen|Weapons", DisplayName=WeaponFiredWithoutAmmo)
	void BP_WeaponFiredWithoutAmmo();
	virtual void WeaponFiredWithoutAmmo();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, Category="Limen|Weapons", DisplayName=WeaponFired)
	void BP_StartWeaponFire();
	virtual void StartWeaponFire();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, Category="Limen|Weapons", DisplayName=WeaponFired)
	void BP_StopWeaponFire();
	virtual void StopWeaponFire();

	virtual void Fire();
	UFUNCTION()
	virtual void Reload(ULimenInventoryComponent* PlayerInventory);

	UFUNCTION()
	virtual void OnRep_CurrentAmmo();
	
private:
	UPROPERTY(Replicated)
	bool bIsHoldingTrigger;
	UPROPERTY(Replicated)
	bool bIsFireRateCooldownOver;
	UPROPERTY(Replicated)
	bool bIsFiring;
	FTimerHandle CooldownTimer;
	FTimerHandle ReloadTimer;
	UPROPERTY(Replicated)
	bool bIsReloading;
	UPROPERTY(Replicated)
	bool bIsNextShotReady;

	int32 ShotsInARow;

	TWeakObjectPtr<UCameraShakeBase> RecoilCameraShakePtr;

	UPROPERTY(Replicated)
	TObjectPtr<ULimenWeaponFireMethod> FireMethodObject;

	UPROPERTY(ReplicatedUsing=OnRep_CurrentAmmo, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	int32 CurrentAmmo;

	double FireAccumulator;
	uint64 SimulatingShotsCount;

	void HandleWeaponCooldown();
	void SetNextShotReady();
	void StartReloadTimer(ULimenInventoryComponent* PlayerInventory);
	void StopReloadTimer();
	void ReloadInternal(ULimenInventoryComponent* PlayerInventory);
	
	virtual void StartFireCameraShake();
	virtual void StopFireCameraShake();

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
