// Copyright Face Software. All Rights Reserved.


#include "Actors/LimenWeapon.h"

#include "AISystem.h"
#include "Actors/LimenAmmo.h"
#include "Camera/CameraComponent.h"
#include "Components/LimenCameraTiltComponent.h"
#include "Components/LimenInventoryComponent.h"
#include "LogMacros/LimenLogMacros.h"
#include "LogMacros/LimenInteractionLogMacros.h"
#include "Perception/AIPerceptionSystem.h"
#include "Perception/AISense_Hearing.h"


ALimenWeapon::ALimenWeapon() : Super()
{
	PrimaryActorTick.bCanEverTick = true;

	AimDownSightsCamera = CreateDefaultSubobject<ULimenCameraTiltComponent>(TEXT("AimDownSightsCamera"));
	check(AimDownSightsCamera != nullptr);
	AimDownSightsCamera->SetupAttachment(GetRootComponent());

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
}

void ALimenWeapon::BeginPlay()
{
	Super::BeginPlay();

	if (GetWorld()->IsGameWorld())
	{
		TimeBetweenShots = 1 / static_cast<double>(RoundsPerSecond);
		ensureAlways(TimeBetweenShots > 0);
		
		UAISystem* AISystem = CastChecked<UAISystem>(GetWorld()->GetAISystem());
		AIPerceptionSystem = AISystem->GetPerceptionSystem();
		check(AIPerceptionSystem != nullptr);
	}
}

void ALimenWeapon::Drop()
{
	OwningController = nullptr;
	OwningPawn = nullptr;
	SetOwner(nullptr);
	CurrentWeaponState = EWeaponState::None;
	OnWeaponStateUpdated.Broadcast(this, CurrentWeaponState);
}

void ALimenWeapon::StartFiring()
{
	bIsHoldingTrigger = true;
	if (!CanFire())
	{
		if (CurrentAmmo <= 0)
		{
			OnWeaponFiredWithoutAmmo();
		}
		
		return;
	}
	
	bIsFiring = true;
	GetWorld()->GetTimerManager().SetTimer(FireRateTimer, this, &ThisClass::Fire, TimeBetweenShots, bIsAutomatic, 0.f);

	LIMEN_LOG(LogLimenInteraction, Log, this, "Started firing")
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
	
	LIMEN_LOG(LogLimenInteraction, Log, this, "Stopped firing")
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

double ALimenWeapon::GetSecondsPerShot() const
{
	return TimeBetweenShots;
}

double ALimenWeapon::GetReloadTime() const
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

double ALimenWeapon::GetBaseDamage() const
{
	return BaseDamage;
}

AController* ALimenWeapon::GetOwningController() const
{
	return OwningController.Get();
}

APawn* ALimenWeapon::GetOwningPawn() const
{
	return OwningPawn.Get();
}

bool ALimenWeapon::HasGameRelevantOwner() const
{
	return GetOwningPawn() && GetOwningController();
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

UCameraComponent* ALimenWeapon::GetAimDownSightsCamera() const
{
	return AimDownSightsCamera.Get();
}

void ALimenWeapon::DecrementAmmo(const int Value)
{
	CurrentAmmo -= Value;
	OnAmmoUpdated.Broadcast(CurrentAmmo);
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
	
	DecrementAmmo();
	OnWeaponFired();
	FireMethod();
}

void ALimenWeapon::Interact(AController* InController, APawn* InPawn)
{
	Super::Interact(InController, InPawn);

	CurrentWeaponState = EWeaponState::Active;
	OnWeaponStateUpdated.Broadcast(this, CurrentWeaponState);
}

void ALimenWeapon::HandleWeaponCooldown()
{
	bIsFireRateCooldownOver = true;
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

	OnReloadStart(ReloadTimeInSeconds);
	OnWeaponReload.Broadcast(this, GetReloadTime());
}

void ALimenWeapon::StopReloadTimer()
{
	bIsReloading = false;
	GetWorld()->GetTimerManager().ClearTimer(ReloadTimer);
}
