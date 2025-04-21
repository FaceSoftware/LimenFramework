// Copyright Face Software. All Rights Reserved.


#include "Actors/LimenWeapon.h"

#include <DamageTypes/LimenDamageType.h>

#include "AISystem.h"
#include "TimerManager.h"
#include "Actors/LimenAmmo.h"
#include "Camera/CameraComponent.h"
#include "Camera/CameraShakeBase.h"
#include "Components/LimenInventoryComponent.h"
#include "FireMethods/LimenWeaponFireMethod.h"
#include "GameFramework/Pawn.h"
#include "Perception/AIPerceptionSystem.h"
#include "Perception/AISense_Hearing.h"


ALimenWeapon::ALimenWeapon() : Super()
{
	PrimaryActorTick.bCanEverTick = true;

	FireMethodClass = ULimenDamageType::StaticClass();
	DamageType = ULimenDamageType::StaticClass();
	BaseDamage = 0.f;
	RoundsPerSecond = 0.f;
	TimeBetweenShots = 0.f;
	MagazineCapacity = 0;
	CurrentAmmo = 0;
	bIsHoldingTrigger = false;
	bIsFireRateCooldownOver = true;
	bIsReloading = false;
	CurrentWeaponState = EWeaponState::None;
	bIsAutomatic = false;
	bIsNextShotReady = true;
	bIsFiring = false;
	ReloadTimeInSeconds = 0;
	WeaponRange = 0;
	ImpactDamageFalloffMultiplier = 0;
	FireSoundRange = 5000;
	RecoilCameraShakeScale = 1.f;
	AINoiseEventLoudness = 0;
}

void ALimenWeapon::BeginPlay()
{
	Super::BeginPlay();

	ULimenWeaponFireMethod* FireMethodInstance = NewObject<ULimenWeaponFireMethod>(
		this, FireMethodClass.LoadSynchronous());
	FireMethodObject = TStrongObjectPtr(FireMethodInstance);

	if (GetWorld()->IsGameWorld())
	{
		TimeBetweenShots = 1 / static_cast<float>(RoundsPerSecond);
		ensureAlways(TimeBetweenShots > 0);
	}
}

void ALimenWeapon::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	FireMethodObject.Reset();
	
	Super::EndPlay(EndPlayReason);
}

void ALimenWeapon::Drop()
{
	SetOwner(nullptr);
	CurrentWeaponState = EWeaponState::None;
	OnWeaponStateUpdated.Broadcast(this, CurrentWeaponState);
}

void ALimenWeapon::PickUp(AController* InController, APawn* InPawn)
{
	Super::PickUp(InController, InPawn);

	CurrentWeaponState = EWeaponState::Active;
	OnWeaponStateUpdated.Broadcast(this, CurrentWeaponState);
}

void ALimenWeapon::StartFiring()
{
	bIsHoldingTrigger = true;
	if (!CanFire())
	{
		if (CurrentAmmo <= 0)
		{
			WeaponFiredWithoutAmmo();
		}
		
		return;
	}
	
	bIsFiring = true;
	GetWorld()->GetTimerManager().SetTimer(FireRateTimer, this, &ThisClass::Fire, TimeBetweenShots, bIsAutomatic, 0.f);
}

void ALimenWeapon::StopFiring()
{
	if (!bIsHoldingTrigger)
	{
		return;
	}

	if (bIsAutomatic)
	{
		GetWorld()->GetTimerManager().ClearTimer(FireRateTimer);
	}

	bIsHoldingTrigger = false;
	bIsFiring = false;
}

bool ALimenWeapon::StartReloading(ULimenInventoryComponent* PlayerInventory)
{
	if (!CanReload(PlayerInventory))
	{
		return false;
	}
	
	StopFiring();
	StartReloadTimer(PlayerInventory);
	return true;
}

void ALimenWeapon::StopReloading()
{
	if (bIsReloading)
	{
		StopReloadTimer();
	}
}

bool ALimenWeapon::IsFiring() const
{
	return bIsFiring;
}

float ALimenWeapon::GetSecondsPerShot() const
{
	return TimeBetweenShots;
}

float ALimenWeapon::GetReloadTime() const
{
	return ReloadTimeInSeconds;
}

bool ALimenWeapon::IsReloading() const
{
	return bIsReloading;
}

bool ALimenWeapon::CanReload(const ULimenInventoryComponent* PlayerInventory) const
{
	return !bIsReloading && CurrentAmmo != MagazineCapacity && PlayerInventory->GetItemQuantity(CompatibleAmmo) != 0;
}

bool ALimenWeapon::CanFire() const
{
	if  (!bIsHoldingTrigger || bIsReloading)
	{
		return false;
	}

	if (!bIsFireRateCooldownOver)
	{
		return false;
	}
	
	
	return CurrentAmmo > 0;
}

float ALimenWeapon::GetBaseDamage() const
{
	return BaseDamage;
}

float ALimenWeapon::GetWeaponRange() const
{
	return WeaponRange;
}

TSubclassOf<ULimenDamageType> ALimenWeapon::GetDamageType() const
{
	return DamageType;
}

float ALimenWeapon::GetImpactDamageFalloffMultiplier() const
{
	return ImpactDamageFalloffMultiplier;
}

const TSubclassOf<ALimenAmmo>& ALimenWeapon::GetCompatibleAmmo() const
{
	return CompatibleAmmo;
}

uint8 ALimenWeapon::GetCurrentAmmo() const
{
	return CurrentAmmo;
}

uint8 ALimenWeapon::GetAmmoCountUntilFull() const
{
	check(MagazineCapacity >= CurrentAmmo);
	return MagazineCapacity - CurrentAmmo;
}

void ALimenWeapon::DecrementAmmo(const int Value)
{
	CurrentAmmo -= Value;
	OnAmmoUpdated.Broadcast(CurrentAmmo);
}

void ALimenWeapon::WeaponFired()
{
	if (const APawn* Pawn = GetOwner<APawn>(); Pawn && Pawn->IsPlayerControlled())
	{
		if (RecoilCameraShakeClass)
		{
			const APlayerController* PC = Pawn->GetController<APlayerController>();
			PC->PlayerCameraManager->StartCameraShake(RecoilCameraShakeClass, RecoilCameraShakeScale);
		}
	}

	UAISystem* AISystem = CastChecked<UAISystem>(GetWorld()->GetAISystem());
	if (UAIPerceptionSystem* AIPerceptionSystem = AISystem->GetPerceptionSystem())
	{
		const FAINoiseEvent NoiseEvent(this, GetActorLocation(), AINoiseEventLoudness, FireSoundRange);
		AIPerceptionSystem->OnEvent(NoiseEvent);
	}
}

void ALimenWeapon::Fire()
{
	if (CurrentAmmo <= 0 || bIsReloading)
	{
		GetWorld()->GetTimerManager().ClearTimer(FireRateTimer);
		bIsFiring = false;
		return;
	}

	DecrementAmmo();
	check(FireMethodObject.IsValid())
	FireMethodObject->ProcessFire(this);

	WeaponFired();
	BP_WeaponFired();
	
	bIsFireRateCooldownOver = false;
	GetWorld()->GetTimerManager().SetTimer(CooldownTimer, this, &ThisClass::HandleWeaponCooldown, TimeBetweenShots, false);
	
	OnWeaponFired.Broadcast(this);
}

void ALimenWeapon::HandleWeaponCooldown()
{
	bIsFireRateCooldownOver = true;
	if (!bIsAutomatic)
	{
		GetWorld()->GetTimerManager().ClearTimer(FireRateTimer);
		bIsFiring = false;
	}

	OnWeaponCooldownOver.Broadcast(this);
}

void ALimenWeapon::SetNextShotReady()
{
	bIsNextShotReady = true;
}

void ALimenWeapon::Reload(ULimenInventoryComponent* PlayerInventory)
{
	const uint8 AmmoMissingInMagazine = MagazineCapacity - CurrentAmmo;
	const int32 InventoryAmmo = PlayerInventory->GetItemQuantity(CompatibleAmmo);

	if (InventoryAmmo >= AmmoMissingInMagazine)
	{
		PlayerInventory->GetItem(CompatibleAmmo, AmmoMissingInMagazine);
		CurrentAmmo = MagazineCapacity;
	}
	else if (InventoryAmmo < AmmoMissingInMagazine)
	{
		PlayerInventory->GetItem(CompatibleAmmo, InventoryAmmo);
		CurrentAmmo += InventoryAmmo;
	}
	
	bIsReloading = false;
	OnAmmoUpdated.Broadcast(CurrentAmmo);
}


void ALimenWeapon::StartReloadTimer(ULimenInventoryComponent* PlayerInventory)
{
	bIsReloading = true;
	const FTimerDelegate ReloadDelegate = FTimerDelegate::CreateUObject(this, &ThisClass::Reload, PlayerInventory);
	GetWorld()->GetTimerManager().SetTimer(ReloadTimer, ReloadDelegate, ReloadTimeInSeconds, false);

	ReloadStart(ReloadTimeInSeconds);
	OnWeaponReload.Broadcast(this, GetReloadTime());
}

void ALimenWeapon::StopReloadTimer()
{
	bIsReloading = false;
	GetWorld()->GetTimerManager().ClearTimer(ReloadTimer);
}
