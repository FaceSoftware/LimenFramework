// Copyright Face Software. All Rights Reserved.


#include "Actors/LimenWeapon.h"

#include "DamageTypes/LimenDamageType.h"
#include "AISystem.h"
#include "TimerManager.h"
#include "Actors/LimenAmmo.h"
#include "Camera/CameraComponent.h"
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
	TimeBetweenShots = 0.f;
	MagazineCapacity = 0;
	InitialAmmo = 0;
	bIsSilenced = false;

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
	RecoilCameraShakeScale = 1.f;
	AINoiseEventLoudness = 0;
	CurrentAmmo = 0;
	ShotsInARow = 0;
}

void ALimenWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);


	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, FireMethodObject,
		FDoRepLifetimeParams(COND_None, REPNOTIFY_OnChanged, true))

	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, CurrentAmmo,
		FDoRepLifetimeParams(COND_None, REPNOTIFY_OnChanged, true))
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, TimeBetweenShots,
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

		FireMethodObject = NewObject<ULimenWeaponFireMethod>(this, FireMethodClass.LoadSynchronous());
		AddReplicatedSubObject(FireMethodObject.Get());

		if (GetWorld()->IsGameWorld())
		{
			TimeBetweenShots = 1 / static_cast<float>(RoundsPerSecond);
			ensureAlways(TimeBetweenShots > 0);
		}
	}
}

void ALimenWeapon::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	RemoveReplicatedSubObject(FireMethodObject.Get());
	FireMethodObject->ConditionalBeginDestroy();
	FireMethodObject = nullptr;

	Super::EndPlay(EndPlayReason);
}

void ALimenWeapon::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (HasAuthority() && bIsHoldingTrigger)
	{
		if (bIsAutomatic || (!bIsAutomatic && ShotsInARow == 0))
		{
			if (!GetWorld()->GetTimerManager().IsTimerActive(FireRateTimer))
			{
				GetWorld()->GetTimerManager().SetTimer(FireRateTimer, this, &ThisClass::Fire,
					TimeBetweenShots, bIsAutomatic, 0.f);
			}
		}
	}
	else if (HasAuthority() && !bIsHoldingTrigger)
	{
		ShotsInARow = 0;
		if (GetWorld()->GetTimerManager().IsTimerActive(FireRateTimer))
		{
			GetWorld()->GetTimerManager().ClearTimer(FireRateTimer);
		}
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
}

void ALimenWeapon::StopFiring()
{
	check(HasAuthority())
	bIsHoldingTrigger = false;
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
	return !bIsReloading && CurrentAmmo < MagazineCapacity && PlayerInventory->GetItemQuantity(CompatibleAmmo) > 0;
}

bool ALimenWeapon::CanFire() const
{
	if (bIsReloading)
	{
		return false;
	}

	if (!bIsAutomatic && !bIsFireRateCooldownOver)
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

float ALimenWeapon::GetAINoiseEventLoudness() const
{
	return AINoiseEventLoudness;
}

float ALimenWeapon::GetFireSoundRange() const
{
	return FireSoundRange;
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
	CurrentAmmo -= Value;
	OnAmmoUpdated.Broadcast(CurrentAmmo);
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
	if (const APawn* Pawn = GetOwner<APawn>(); Pawn && Pawn->IsPlayerControlled() && Pawn->IsLocallyControlled())
	{
		if (RecoilCameraShakeClass)
		{
			const APlayerController* PC = Pawn->GetController<APlayerController>();
			PC->PlayerCameraManager->StartCameraShake(RecoilCameraShakeClass, RecoilCameraShakeScale);
		}
	}

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

void ALimenWeapon::Fire()
{
	check(HasAuthority())

	if (!CanFire())
	{
		bIsFiring = false;
		if (CurrentAmmo <= 0)
		{
			WeaponFiredWithoutAmmo();
			BP_WeaponFiredWithoutAmmo();
		}
	}
	else
	{
		bIsFiring = true;
		DecrementAmmo();
		ShotsInARow++;

		check(FireMethodObject)
		FireMethodObject->ProcessFire(this);
		Multicast_WeaponFired();

		bIsFireRateCooldownOver = false;
	}

	if (!GetWorld()->GetTimerManager().IsTimerActive(CooldownTimer))
	{
		GetWorld()->GetTimerManager().SetTimer(CooldownTimer, this, &ThisClass::HandleWeaponCooldown,
			TimeBetweenShots, false);
	}
}

void ALimenWeapon::Reload(ULimenInventoryComponent* PlayerInventory)
{
	check(HasAuthority())

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

	OnAmmoUpdated.Broadcast(CurrentAmmo);
}

void ALimenWeapon::OnRep_CurrentAmmo()
{
	OnAmmoUpdated.Broadcast(CurrentAmmo);
}

void ALimenWeapon::HandleWeaponCooldown()
{
	check(HasAuthority())

	bIsFireRateCooldownOver = true;
	OnWeaponCooldownOver.Broadcast(this);
}

void ALimenWeapon::SetNextShotReady()
{
	bIsNextShotReady = true;
}

void ALimenWeapon::StartReloadTimer(ULimenInventoryComponent* PlayerInventory)
{
	check(HasAuthority())

	bIsReloading = true;
	const FTimerDelegate ReloadDelegate = FTimerDelegate::CreateUObject(this, &ThisClass::ReloadInternal, PlayerInventory);
	GetWorld()->GetTimerManager().SetTimer(ReloadTimer, ReloadDelegate, ReloadTimeInSeconds, false);

	Multicast_ReloadStart();
}

void ALimenWeapon::StopReloadTimer()
{
	bIsReloading = false;
	GetWorld()->GetTimerManager().ClearTimer(ReloadTimer);
}

void ALimenWeapon::ReloadInternal(ULimenInventoryComponent* PlayerInventory)
{
	check(HasAuthority())

	Reload(PlayerInventory);
	StopReloadTimer();
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

void ALimenWeapon::Multicast_WeaponFired_Implementation()
{
	WeaponFired();
	BP_WeaponFired();
}
