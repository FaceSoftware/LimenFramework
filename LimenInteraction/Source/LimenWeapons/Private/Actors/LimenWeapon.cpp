// Copyright Face Software. All Rights Reserved.


#include "Actors/LimenWeapon.h"

#include "DamageTypes/LimenDamageType.h"
#include "AISystem.h"
#include "TimerManager.h"
#include "Actors/LimenAmmo.h"
#include "Camera/CameraShakeBase.h"
#include "Components/LimenInventoryComponent.h"
#include "FireMethods/LimenWeaponFireMethod.h"
#include "GameFramework/Pawn.h"
#include "Net/UnrealNetwork.h"
#include "Perception/AIPerceptionSystem.h"
#include "Perception/AISense_Hearing.h"


ALimenWeapon::ALimenWeapon(const FObjectInitializer& InObjectInitializer) : Super(InObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	bAlwaysRelevant = true;
	bReplicateUsingRegisteredSubObjectList = true;
	SetReplicatingMovement(true);

	FireMethodClass = ULimenDamageType::StaticClass();
	DamageType = ULimenDamageType::StaticClass();
	BaseDamage = 0.f;
	RoundsPerSecond = 0.f;
	MagazineCapacity = 0;
	InitialAmmo = 0;
	bIsSilenced = false;
	InfiniteAmmoType = EInfiniteAmmoType::Disabled;

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
	AINoiseEventLoudness = 0;
	CurrentAmmo = 0;
	ShotsInARow = 0;
	FireAccumulator = 0.0;
	SimulatingShotsCount = 0;
}

void ALimenWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, BaseDamage,
		FDoRepLifetimeParams(COND_None, REPNOTIFY_OnChanged, true))
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, RoundsPerSecond,
		FDoRepLifetimeParams(COND_None, REPNOTIFY_OnChanged, true))
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, MagazineCapacity,
		FDoRepLifetimeParams(COND_None, REPNOTIFY_OnChanged, true))
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, ReloadTimeInSeconds,
		FDoRepLifetimeParams(COND_None, REPNOTIFY_OnChanged, true))
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, bIsAutomatic,
		FDoRepLifetimeParams(COND_None, REPNOTIFY_OnChanged, true))
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, WeaponRange,
		FDoRepLifetimeParams(COND_None, REPNOTIFY_OnChanged, true))
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, ImpactDamageFalloffMultiplier,
		FDoRepLifetimeParams(COND_None, REPNOTIFY_OnChanged, true))
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, FireSoundRange,
		FDoRepLifetimeParams(COND_None, REPNOTIFY_OnChanged, true))
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, AINoiseEventLoudness,
		FDoRepLifetimeParams(COND_None, REPNOTIFY_OnChanged, true))
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, bIsSilenced,
		FDoRepLifetimeParams(COND_None, REPNOTIFY_OnChanged, true))
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, InfiniteAmmoType,
		FDoRepLifetimeParams(COND_None, REPNOTIFY_OnChanged, true))

	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, FireMethodObject,
		FDoRepLifetimeParams(COND_None, REPNOTIFY_OnChanged, true))

	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, CurrentAmmo,
		FDoRepLifetimeParams(COND_None, REPNOTIFY_OnChanged, true))
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, bIsHoldingTrigger,
		FDoRepLifetimeParams(COND_None, REPNOTIFY_OnChanged, true))
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, bIsFireRateCooldownOver,
		FDoRepLifetimeParams(COND_None, REPNOTIFY_OnChanged, true))
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, bIsFiring,
		FDoRepLifetimeParams(COND_None, REPNOTIFY_OnChanged, true))
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, bIsReloading,
		FDoRepLifetimeParams(COND_None, REPNOTIFY_OnChanged, true))
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, bIsNextShotReady,
		FDoRepLifetimeParams(COND_None, REPNOTIFY_OnChanged, true))
}

void ALimenWeapon::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority() && !FireMethodClass.IsNull())
	{
		CurrentAmmo = InitialAmmo;
		MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, CurrentAmmo, this)

		FireMethodObject = NewObject<ULimenWeaponFireMethod>(this, FireMethodClass.LoadSynchronous());
		AddReplicatedSubObject(FireMethodObject.Get());
		MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, FireMethodObject, this)

		if (GetWorld()->IsGameWorld())
		{
			ensureAlways(1 / static_cast<float>(RoundsPerSecond) > 0);
		}
	}
}

void ALimenWeapon::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	RemoveReplicatedSubObject(FireMethodObject.Get());
	FireMethodObject->ConditionalBeginDestroy();
	FireMethodObject = nullptr;

	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, FireMethodObject, this)
	Super::EndPlay(EndPlayReason);
}

void ALimenWeapon::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!HasAuthority()) return;

	if (bIsHoldingTrigger)
	{
		if (bIsAutomatic)
		{
			Fire();
		}
		else
		{
			Fire();
		}
	}
	else
	{
		ShotsInARow = 0;
		FireAccumulator = 0.f;
		SimulatingShotsCount = 0;
	}
}

void ALimenWeapon::Drop(AController* InController, APawn* InPawn)
{
	Super::Drop(InController, InPawn);

	TArray<UPrimitiveComponent*> Components;
	GetComponents<UPrimitiveComponent>(Components);	
	for (UPrimitiveComponent*& Component : Components)
	{
		Component->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		Component->SetSimulatePhysics(true);
	}

	OnWeaponStateUpdated.Broadcast(this, true);
}

void ALimenWeapon::PickUp(AController* InController, APawn* InPawn)
{
	Super::PickUp(InController, InPawn);

	TArray<UPrimitiveComponent*> Components;
	GetComponents<UPrimitiveComponent>(Components);
	for (UPrimitiveComponent* Component : Components)
	{
		Component->SetSimulatePhysics(false);
		Component->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}

	OnWeaponStateUpdated.Broadcast(this, false);
}

void ALimenWeapon::StartFiring()
{
	check(HasAuthority())
	bIsHoldingTrigger = true;
	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, bIsHoldingTrigger, this)
}

void ALimenWeapon::StopFiring()
{
	check(HasAuthority())
	bIsHoldingTrigger = false;
	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, bIsHoldingTrigger, this)
}

void ALimenWeapon::StartReloading(ULimenInventoryComponent* PlayerInventory)
{
	check(HasAuthority())

	if (!CanReload(PlayerInventory))
	{
		return;
	}

	StartReloadTimer(PlayerInventory);
}

void ALimenWeapon::StopReloading()
{
	check(HasAuthority())

	if (bIsReloading)
	{
		StopReloadTimer();
	}

	Multicast_ReloadCanceled();
}

bool ALimenWeapon::IsFiring() const
{
	return bIsFiring;
}

float ALimenWeapon::GetSecondsPerShot() const
{
	return 1 / static_cast<float>(RoundsPerSecond);
}

float ALimenWeapon::GetFireRate() const
{
	return RoundsPerSecond;
}

void ALimenWeapon::SetFireRate(const float InFireRate)
{
	RoundsPerSecond = InFireRate;
	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, BaseDamage, this)
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
	return !bIsReloading && CurrentAmmo < MagazineCapacity && PlayerInventory->GetItemQuantity(CompatibleAmmo) > 0;
}

bool ALimenWeapon::CanFire() const
{
	if (bIsReloading)
	{
		return false;
	}

	if (!bIsAutomatic && ShotsInARow > 0)
	{
		return false;
	}

	if (!bIsFireRateCooldownOver)
	{
		return false;
	}

	switch (InfiniteAmmoType)
	{
	case EInfiniteAmmoType::InfiniteBullets:
		return true;

	default:
		break;
	}

	return CurrentAmmo > 0;
}

float ALimenWeapon::GetBaseDamage() const
{
	return BaseDamage;
}

void ALimenWeapon::SetBaseDamage(const float InNewDamage)
{
	BaseDamage = InNewDamage;
	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, BaseDamage, this)
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

float ALimenWeapon::GetAINoiseEventLoudness() const
{
	return AINoiseEventLoudness;
}

float ALimenWeapon::GetFireSoundRange() const
{
	return FireSoundRange;
}

EInfiniteAmmoType ALimenWeapon::GetInfiniteAmmoType() const
{
	return InfiniteAmmoType;
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

FVector ALimenWeapon::GetFiringLocation_Implementation() const
{
	return FVector::ZeroVector;
}

void ALimenWeapon::OnRep_IsDropped()
{
	Super::OnRep_IsDropped();

	TArray<UPrimitiveComponent*> Components;
	GetComponents<UPrimitiveComponent>(Components);
	for (auto& Component : Components)
	{
		Component->SetSimulatePhysics(IsDropped());
	}

	OnWeaponStateUpdated.Broadcast(this, IsDropped());
}

void ALimenWeapon::DecrementAmmo(const int Value)
{
	check(HasAuthority())

	switch (InfiniteAmmoType)
	{
	case EInfiniteAmmoType::InfiniteMagazines:
	case EInfiniteAmmoType::Disabled:
		{
			CurrentAmmo -= Value;
			OnAmmoUpdated.Broadcast(CurrentAmmo);
		}
		break;
		
	case EInfiniteAmmoType::InfiniteBullets:
		{
		}
		break;

	default:
		{
		}
		break;
	}
}

void ALimenWeapon::ReloadStart(const float ReloadTimeSeconds)
{
	OnWeaponReload.Broadcast(this, GetReloadTime());
}

void ALimenWeapon::ReloadCancelled(const float ReloadTimeSeconds)
{
}

void ALimenWeapon::WeaponFired()
{
	StartFireCameraShake();
	StopFireCameraShake();

	if (HasAuthority() && !bIsSilenced)
	{
		UAISystem* AISystem = CastChecked<UAISystem>(GetWorld()->GetAISystem());
		if (UAIPerceptionSystem* AIPerceptionSystem = AISystem->GetPerceptionSystem())
		{
			const FAINoiseEvent NoiseEvent(this, GetActorLocation(), GetAINoiseEventLoudness(),
										   GetFireSoundRange());

			AIPerceptionSystem->OnEvent(NoiseEvent);
		}
	}

	OnWeaponFired.Broadcast(this);
}

void ALimenWeapon::WeaponFiredWithoutAmmo()
{
}

void ALimenWeapon::StartWeaponFire()
{
}

void ALimenWeapon::StopWeaponFire()
{
}

void ALimenWeapon::Fire()
{
	check(HasAuthority())

	if (!CanFire())
	{
		if (bIsFiring)
		{
			Multicast_StopWeaponFire();
		}

		bIsFiring = false;
		MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, bIsFireRateCooldownOver, this)
		if (CurrentAmmo <= 0)
		{
			WeaponFiredWithoutAmmo();
			BP_WeaponFiredWithoutAmmo();
		}
	}
	else
	{
		FireAccumulator += RoundsPerSecond * GetWorld()->GetDeltaSeconds();
		SimulatingShotsCount = FMath::FloorToInt(FireAccumulator);\
		if (SimulatingShotsCount == 0)
		{
			SimulatingShotsCount = 1;
		}
		else
		{
			FireAccumulator -= SimulatingShotsCount;
		}

		if (SimulatingShotsCount > 0)
		{
			if (!bIsFiring)
			{
				bIsFiring = true;
				MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, bIsFireRateCooldownOver, this)
				Multicast_StartWeaponFire();
			}

			DecrementAmmo(SimulatingShotsCount);
			ShotsInARow += SimulatingShotsCount;

			check(FireMethodObject)
			FireMethodObject->ProcessFire(this, SimulatingShotsCount);
			Multicast_WeaponFired();

			bIsFireRateCooldownOver = false;
			MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, bIsFireRateCooldownOver, this)
		}
	}

	if (!GetWorld()->GetTimerManager().IsTimerActive(CooldownTimer))
	{
		GetWorld()->GetTimerManager().SetTimer(CooldownTimer, this, &ThisClass::HandleWeaponCooldown,
			1.f / RoundsPerSecond, false);
	}
}

void ALimenWeapon::Reload(ULimenInventoryComponent* PlayerInventory)
{
	check(HasAuthority())

	switch (InfiniteAmmoType)
	{
	case EInfiniteAmmoType::InfiniteBullets: // Fallback
	case EInfiniteAmmoType::InfiniteMagazines:
		{
			CurrentAmmo = MagazineCapacity;
			MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, CurrentAmmo, this)
		}
		break;

	case EInfiniteAmmoType::Disabled:
		{
			const uint8 AmmoMissingInMagazine = MagazineCapacity - CurrentAmmo;
			const int32 InventoryAmmo = PlayerInventory->GetItemQuantity(CompatibleAmmo);

			if (InventoryAmmo >= AmmoMissingInMagazine)
			{
				PlayerInventory->GetItems(CompatibleAmmo, AmmoMissingInMagazine);
				CurrentAmmo = MagazineCapacity;
				MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, CurrentAmmo, this)
			}
			else if (InventoryAmmo < AmmoMissingInMagazine)
			{
				PlayerInventory->GetItems(CompatibleAmmo, InventoryAmmo);
				CurrentAmmo += InventoryAmmo;
				MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, CurrentAmmo, this)
			}

			OnAmmoUpdated.Broadcast(CurrentAmmo);
		}
		break;

	default:
		{
		}
		break;
	}
}

void ALimenWeapon::OnRep_CurrentAmmo()
{
	OnAmmoUpdated.Broadcast(CurrentAmmo);
}

void ALimenWeapon::HandleWeaponCooldown()
{
	check(HasAuthority())

	bIsFireRateCooldownOver = true;
	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, bIsFireRateCooldownOver, this)
	OnWeaponCooldownOver.Broadcast(this);
}

void ALimenWeapon::SetNextShotReady()
{
	bIsNextShotReady = true;
	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, bIsNextShotReady, this)
}

void ALimenWeapon::StartReloadTimer(ULimenInventoryComponent* PlayerInventory)
{
	check(HasAuthority())

	bIsReloading = true;
	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, bIsFireRateCooldownOver, this)
	const FTimerDelegate ReloadDelegate = FTimerDelegate::CreateUObject(this, &ThisClass::ReloadInternal, PlayerInventory);
	GetWorld()->GetTimerManager().SetTimer(ReloadTimer, ReloadDelegate, ReloadTimeInSeconds, false);

	Multicast_ReloadStart();
}

void ALimenWeapon::StopReloadTimer()
{
	bIsReloading = false;
	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, bIsFireRateCooldownOver, this)
	GetWorld()->GetTimerManager().ClearTimer(ReloadTimer);
}

void ALimenWeapon::ReloadInternal(ULimenInventoryComponent* PlayerInventory)
{
	check(HasAuthority())

	Reload(PlayerInventory);
	StopReloadTimer();
}

void ALimenWeapon::StartFireCameraShake()
{
	if (!RecoilCameraShakeClass)
	{
		return;
	}

	const APawn* Pawn = GetOwner<APawn>();
	if (!Pawn || !Pawn->IsPlayerControlled() || !Pawn->IsLocallyControlled())
	{
		return;
	}

	const APlayerController* PC = Pawn->GetController<APlayerController>();
	if (!PC)
	{
		return;
	}

	if (RecoilCameraShakePtr.IsValid() && RecoilCameraShakePtr->IsActive())
	{
		PC->PlayerCameraManager->StopCameraShake(RecoilCameraShakePtr.Get(), true);
		RecoilCameraShakePtr.Reset();
	}
	
	RecoilCameraShakePtr = PC->PlayerCameraManager->StartCameraShake(RecoilCameraShakeClass);
}

void ALimenWeapon::StopFireCameraShake()
{
	if (!RecoilCameraShakePtr.IsValid())
	{
		return;
	}

	if (const APawn* Pawn = GetOwner<APawn>(); Pawn && Pawn->IsPlayerControlled() && Pawn->IsLocallyControlled())
	{
		if (const APlayerController* PC = Pawn->GetController<APlayerController>();
			PC && RecoilCameraShakePtr->IsActive())
		{
			PC->PlayerCameraManager->StopCameraShake(RecoilCameraShakePtr.Get(), false);
		}
	}
}

void ALimenWeapon::Multicast_ReloadCanceled_Implementation()
{
	ReloadCancelled(ReloadTimeInSeconds);
	BP_ReloadCancelled(ReloadTimeInSeconds);
	OnWeaponReloadCanceled.Broadcast(this, ReloadTimeInSeconds);
}

void ALimenWeapon::Multicast_ReloadStart_Implementation()
{
	ReloadStart(ReloadTimeInSeconds);
	BP_ReloadStart(ReloadTimeInSeconds);
}

void ALimenWeapon::Multicast_StartWeaponFire_Implementation()
{
	StartWeaponFire();
}

void ALimenWeapon::Multicast_StopWeaponFire_Implementation()
{
	StopWeaponFire();
}

void ALimenWeapon::Multicast_WeaponFired_Implementation()
{
	WeaponFired();
	BP_WeaponFired();
}
