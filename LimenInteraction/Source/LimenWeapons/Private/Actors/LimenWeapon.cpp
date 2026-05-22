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
#include "Utils/LimenReplicationUtils.h"


ALimenWeapon::ALimenWeapon(const FObjectInitializer& InObjectInitializer) : Super(InObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	bAlwaysRelevant = true;
	SetReplicatingMovement(true);
	SetNetUpdateFrequency(20.f);
	SetMinNetUpdateFrequency(5.f);

	FireMethodClass = ULimenDamageType::StaticClass();
	DamageType = ULimenDamageType::StaticClass();
	BaseDamage = 0.f;
	SecondsPerShot = 0.f;
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

	FDoRepLifetimeParams Params(COND_None, REPNOTIFY_OnChanged, true);

	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, BaseDamage, Params)
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, SecondsPerShot, Params)
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, MagazineCapacity, Params)
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, ReloadTimeInSeconds, Params)
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, bIsAutomatic, Params)
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, WeaponRange, Params)
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, ImpactDamageFalloffMultiplier, Params)
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, FireSoundRange, Params)
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, AINoiseEventLoudness, Params)
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, bIsSilenced, Params)
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, InfiniteAmmoType, Params)
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, CurrentAmmo, Params)
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, bIsHoldingTrigger, Params)
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, bIsFireRateCooldownOver, Params)
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, bIsFiring, Params)
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, bIsReloading, Params)
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, bIsNextShotReady, Params)
}

void ALimenWeapon::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		CurrentAmmo = InitialAmmo;
		MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, CurrentAmmo, this)

		if (!FireMethodClass.IsNull())
		{
			FireMethodObject = TStrongObjectPtr(NewObject<ULimenWeaponFireMethod>(this, FireMethodClass.LoadSynchronous()));
		}

		if (GetWorld()->IsGameWorld())
		{
			ensureAlways(!FMath::IsNegativeOrNegativeZero(SecondsPerShot));
		}
	}
}

void ALimenWeapon::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (FireMethodObject)
	{
		FireMethodObject->ConditionalBeginDestroy();
		FireMethodObject.Reset();
	}

	Super::EndPlay(EndPlayReason);
}

void ALimenWeapon::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bIsHoldingTrigger)
	{
		Fire();
	}
	else
	{
		ShotsInARow = 0;
		FireAccumulator = 0.f;
		SimulatingShotsCount = 0;
	}
}

void ALimenWeapon::StartFiring()
{
	NETWORK_PREDICTION(SetFireStateInternal(true), Server_SetFireState(true))
}

void ALimenWeapon::StopFiring()
{
	NETWORK_PREDICTION(SetFireStateInternal(false), Server_SetFireState(false))
}

void ALimenWeapon::StartReloading()
{
	NETWORK_PREDICTION(SetReloadStateInternal(true), Server_SetReloadState(true))
}

void ALimenWeapon::StopReloading()
{
	NETWORK_PREDICTION(SetReloadStateInternal(false), Server_SetReloadState(false))
}

bool ALimenWeapon::IsFiring() const
{
	return bIsFiring;
}

float ALimenWeapon::GetSecondsPerShot() const
{
	return SecondsPerShot;
}

void ALimenWeapon::SetSecondsPerShot(const float NewValue)
{
	SecondsPerShot = NewValue;
	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, SecondsPerShot, this)
}

float ALimenWeapon::GetReloadTime() const
{
	return ReloadTimeInSeconds;
}

void ALimenWeapon::SetReloadTime(const float NewValue)
{
	ReloadTimeInSeconds = NewValue;
	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, ReloadTimeInSeconds, this)
}

bool ALimenWeapon::IsReloading() const
{
	return bIsReloading;
}

bool ALimenWeapon::CanReload() const
{
	return !bIsReloading && CurrentAmmo < MagazineCapacity;
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

void ALimenWeapon::SetBaseDamage(const float NewValue)
{
	BaseDamage = NewValue;
	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, BaseDamage, this)
}

float ALimenWeapon::GetWeaponRange() const
{
	return WeaponRange;
}

void ALimenWeapon::SetWeaponRange(const float NewValue)
{
	WeaponRange = NewValue;
	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, WeaponRange, this)
}

TSubclassOf<ULimenDamageType> ALimenWeapon::GetDamageType() const
{
	return DamageType;
}

void ALimenWeapon::SetDamageType(const TSubclassOf<ULimenDamageType>& NewValue)
{
	DamageType = NewValue;
}

float ALimenWeapon::GetImpactDamageFalloffMultiplier() const
{
	return ImpactDamageFalloffMultiplier;
}

void ALimenWeapon::SetImpactDamageFalloffMultiplier(const float NewValue)
{
	ImpactDamageFalloffMultiplier = NewValue;
	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, ImpactDamageFalloffMultiplier, this)
}

float ALimenWeapon::GetAINoiseEventLoudness() const
{
	return AINoiseEventLoudness;
}

void ALimenWeapon::SetAINoiseEventLoudness(const float NewValue)
{
	AINoiseEventLoudness = NewValue;
	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, AINoiseEventLoudness, this)
}

float ALimenWeapon::GetFireSoundRange() const
{
	return FireSoundRange;
}

void ALimenWeapon::SetFireSoundRange(const float NewValue)
{
	FireSoundRange = NewValue;
	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, FireSoundRange, this)
}

EInfiniteAmmoType ALimenWeapon::GetInfiniteAmmoType() const
{
	return InfiniteAmmoType;
}

void ALimenWeapon::SetInfiniteAmmoType(const EInfiniteAmmoType NewValue)
{
	InfiniteAmmoType = NewValue;
	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, InfiniteAmmoType, this)
}

FVector ALimenWeapon::GetFiringLocation() const
{
	if(!GetOwner())
	{
		return FVector::ZeroVector;
	}

	FVector Location;
	FRotator Rotation;
	GetOwner()->GetActorEyesViewPoint(Location, Rotation);

	return Location;
}

TSubclassOf<ALimenAmmo> ALimenWeapon::GetCompatibleAmmo() const
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

void ALimenWeapon::Dropped(AController* InController, APawn* InPawn)
{
	Super::Dropped(InController, InPawn);

	TArray<UPrimitiveComponent*> Components;
	GetComponents<UPrimitiveComponent>(Components);	
	for (UPrimitiveComponent*& Component : Components)
	{
		Component->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		Component->SetSimulatePhysics(true);
	}
}

void ALimenWeapon::PickedUp(AController* InController, APawn* InPawn)
{
	Super::PickedUp(InController, InPawn);

	TArray<UPrimitiveComponent*> Components;
	GetComponents<UPrimitiveComponent>(Components);
	for (UPrimitiveComponent* Component : Components)
	{
		Component->SetSimulatePhysics(false);
		Component->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}
}

void ALimenWeapon::DecrementAmmo(const int Value)
{
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

void ALimenWeapon::ReloadStart()
{
	OnWeaponReload.Broadcast(this, GetReloadTime());
}

void ALimenWeapon::ReloadCancelled()
{
	OnWeaponReloadCanceled.Broadcast(this, ReloadTimeInSeconds);
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
	if (!CanFire())
	{
		if (bIsFiring)
		{
			bIsFiring = false;
			MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, bIsFiring, this)

			if (GetNetMode() != NM_Standalone && HasAuthority())
			{
				StopWeaponFire();
			}
			else
			{
				Multicast_StopWeaponFire();
			}
				
		}

		if (CurrentAmmo <= 0)
		{
			WeaponFiredWithoutAmmo();
		}
	}
	else
	{
		FireAccumulator += (1.f / SecondsPerShot) * GetWorld()->GetDeltaSeconds();
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
				MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, bIsFiring, this)

				if (GetNetMode() != NM_Standalone && HasAuthority())
				{
					Multicast_StartWeaponFire();
				}
				else
				{
					StartWeaponFire();
				}
			}

			DecrementAmmo(SimulatingShotsCount);
			ShotsInARow += SimulatingShotsCount;

			if (HasAuthority())
			{
				check(FireMethodObject)
				FireMethodObject->ProcessFire(this, SimulatingShotsCount);
			}

			if (GetNetMode() != NM_Standalone && HasAuthority())
			{
				Multicast_WeaponFired();
			}
			else
			{
				WeaponFired();
			}

			bIsFireRateCooldownOver = false;
			MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, bIsFireRateCooldownOver, this)
		}
	}

	if (!FMath::IsNearlyZero(SecondsPerShot))
	{
		if (!GetWorld()->GetTimerManager().IsTimerActive(CooldownTimer))
		{
			GetWorld()->GetTimerManager().SetTimer(CooldownTimer, this, &ThisClass::HandleWeaponCooldown,
				SecondsPerShot, false);
		}
	}
	else
	{
		HandleWeaponCooldown();
	}
}

void ALimenWeapon::Reload()
{
	switch (InfiniteAmmoType)
	{
	case EInfiniteAmmoType::InfiniteBullets:
	case EInfiniteAmmoType::InfiniteMagazines:
		{
			CurrentAmmo = MagazineCapacity;
			MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, CurrentAmmo, this)
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
	bIsFireRateCooldownOver = true;
	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, bIsFireRateCooldownOver, this)
	OnWeaponCooldownOver.Broadcast(this);
}

void ALimenWeapon::SetNextShotReady()
{
	bIsNextShotReady = true;
	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, bIsNextShotReady, this)
}

void ALimenWeapon::StartReloadTimer()
{
	bIsReloading = true;
	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, bIsReloading, this)
	GetWorld()->GetTimerManager().SetTimer(ReloadTimer, this, &ThisClass::ReloadInternal, ReloadTimeInSeconds, false);
}

void ALimenWeapon::StopReloadTimer()
{
	bIsReloading = false;
	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, bIsReloading, this)
	GetWorld()->GetTimerManager().ClearTimer(ReloadTimer);
}

void ALimenWeapon::ReloadInternal()
{
	Reload();
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

void ALimenWeapon::Server_SetFireState_Implementation(const bool bEnabled)
{
	SetFireStateInternal(bEnabled);
}

void ALimenWeapon::Server_SetReloadState_Implementation(const bool bEnabled)
{
	SetReloadStateInternal(bEnabled);
}

void ALimenWeapon::Multicast_ReloadCanceled_Implementation()
{
	if (const auto* OwnerPawn = GetOwner<APawn>(); OwnerPawn->IsLocallyControlled())
	{
		return;
	}

	ReloadCancelled();
}

void ALimenWeapon::Multicast_ReloadStart_Implementation()
{
	if (const auto* OwnerPawn = GetOwner<APawn>(); OwnerPawn->IsLocallyControlled())
	{
		return;
	}

	ReloadStart();
}

void ALimenWeapon::Multicast_StartWeaponFire_Implementation()
{
	if (const auto* OwnerPawn = GetOwner<APawn>(); OwnerPawn->IsLocallyControlled())
	{
		return;
	}

	StartWeaponFire();
}

void ALimenWeapon::Multicast_StopWeaponFire_Implementation()
{
	if (const auto* OwnerPawn = GetOwner<APawn>(); OwnerPawn->IsLocallyControlled())
	{
		return;
	}

	StopWeaponFire();
}

void ALimenWeapon::Multicast_WeaponFired_Implementation()
{
	if (const auto* OwnerPawn = GetOwner<APawn>(); OwnerPawn->IsLocallyControlled())
	{
		return;
	}

	WeaponFired();
}

void ALimenWeapon::SetFireStateInternal(const bool bEnabled)
{
	if (bEnabled != bIsHoldingTrigger)
	{
		bIsHoldingTrigger = bEnabled;
		MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, bIsHoldingTrigger, this)
	}
}

void ALimenWeapon::SetReloadStateInternal(const bool bEnabled)
{
	if (bEnabled)
	{
		if (!CanReload())
		{
			return;
		}

		StartReloadTimer();
		Multicast_ReloadStart();
	}
	else
	{
		if (bIsReloading)
		{
			StopReloadTimer();
		}

		Multicast_ReloadCanceled();
	}
}
