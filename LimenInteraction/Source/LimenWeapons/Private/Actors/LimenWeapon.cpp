// Copyright Face Software. All Rights Reserved.


#include "Actors/LimenWeapon.h"

#include "AISystem.h"
#include "Actors/LimenAmmo.h"
#include "Net/UnrealNetwork.h"
#include "Perception/AIPerceptionSystem.h"
#include "Perception/AISense_Hearing.h"


ALimenWeapon::ALimenWeapon(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	BaseDamage = 0.f;
	RoundsPerSecond = 0.f;
	TimeBetweenShots = 0.f;
	MagazineCapacity = 0;
	CurrentAmmo = 0;
	bIsHoldingTrigger = false;
	bIsFireRateCooldownOver = true;
	bIsReloading = false;
	bIsAutomatic = false;
	bIsNextShotReady = true;
	bIsFiring = false;
	ReloadTimeInSeconds = 0;
	WeaponRange = 0;
	ImpactDamageFalloffMultiplier = 0;
	FireSoundRange = 5000;
}

void ALimenWeapon::BeginPlay()
{
	Super::BeginPlay();

	if (GetWorld()->IsGameWorld())
	{
		TimeBetweenShots = 1 / static_cast<double>(RoundsPerSecond);
		ensureAlways(TimeBetweenShots > 0);

		if (GetNetMode() < NM_Client)
		{
			UAISystem* AISystem = CastChecked<UAISystem>(GetWorld()->GetAISystem());
			AIPerceptionSystem = AISystem->GetPerceptionSystem();
			check(AIPerceptionSystem != nullptr);
		}
	}
}

void ALimenWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALimenWeapon, CurrentAmmo);
	DOREPLIFETIME(ALimenWeapon, bIsReloading);
	DOREPLIFETIME(ALimenWeapon, bIsFiring);
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

void ALimenWeapon::StartReloading()
{
	check(CanReload());
	
	StopFiring();
	StartReloadTimer();
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

bool ALimenWeapon::CanReload() const
{
	return !bIsReloading && CurrentAmmo != MagazineCapacity;
}

bool ALimenWeapon::CanFire() const
{
	if (!bIsHoldingTrigger || bIsReloading)
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

const TSubclassOf<ALimenAmmo>& ALimenWeapon::GetCompatibleAmmo() const
{
	return CompatibleAmmo;
}

int32 ALimenWeapon::GetCurrentAmmo() const
{
	return CurrentAmmo;
}

int32 ALimenWeapon::GetAmmoCountUntilFull() const
{
	check(MagazineCapacity >= CurrentAmmo);
	return MagazineCapacity - CurrentAmmo;
}

void ALimenWeapon::DecrementAmmo(const int Value)
{
	CurrentAmmo -= Value;
	OnAmmoUpdated.Broadcast(CurrentAmmo);
}

void ALimenWeapon::SetCurrentAmmo(const int32 NewValue)
{
	CurrentAmmo = NewValue;
	OnAmmoUpdated.Broadcast(CurrentAmmo);
}

void ALimenWeapon::OnReloadStart(const float ReloadTimeSeconds)
{
}

void ALimenWeapon::WeaponFired()
{
}

void ALimenWeapon::WeaponFiredWithoutAmmo()
{
}

void ALimenWeapon::Fire()
{
	if (CurrentAmmo <= 0 || bIsReloading)
	{
		GetWorld()->GetTimerManager().ClearTimer(FireRateTimer);
		bIsFiring = false;
		return;
	}
	
	bIsFireRateCooldownOver = false;
	GetWorld()->GetTimerManager().SetTimer(CooldownTimer, this, &ThisClass::HandleWeaponCooldown, TimeBetweenShots, false);

	check(AIPerceptionSystem != nullptr);
	const FAINoiseEvent Event(this, GetActorLocation(), 1, FireSoundRange);
	AIPerceptionSystem->OnEvent(Event);
	
	FireMethod();
	DecrementAmmo();
	WeaponFired();
}

void ALimenWeapon::FireMethod()
{
}

void ALimenWeapon::OnRep_CurrentAmmo()
{
	OnAmmoUpdated.Broadcast(CurrentAmmo);
}

void ALimenWeapon::HandleWeaponCooldown()
{
	bIsFireRateCooldownOver = true;
}

void ALimenWeapon::SetNextShotReady()
{
	bIsNextShotReady = true;
}

void ALimenWeapon::Reload()
{
	bIsReloading = false;
	OnAmmoUpdated.Broadcast(CurrentAmmo);
}

void ALimenWeapon::StartReloadTimer()
{
	bIsReloading = true;
	
	GetWorld()->GetTimerManager().SetTimer(ReloadTimer, this, &ThisClass::Reload, ReloadTimeInSeconds, false);

	OnReloadStart(ReloadTimeInSeconds);
	OnWeaponReload.Broadcast(this, GetReloadTime());
}

void ALimenWeapon::StopReloadTimer()
{
	bIsReloading = false;
	GetWorld()->GetTimerManager().ClearTimer(ReloadTimer);
}
