// Copyright © 2024 FaceSoftware. All rights reserved.


#include "FireMethods/LimenWeaponFireMethod.h"

#include "Components/LimenDamageComponent.h"
#include "DamageTypes/LimenDamageType.h"
#include "AISystem.h"
#include "CollisionQueryParams.h"
#include "DrawDebugHelpers.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Actors/LimenWeapon.h"
#include "Components/DecalComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/EngineTypes.h"
#include "Engine/HitResult.h"
#include "Engine/NetDriver.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "Iris/ReplicationSystem/ReplicationSystem.h"
#include "Kismet/GameplayStatics.h"
#include "LogMacros/LimenLogMacros.h"
#include "Perception/AIPerceptionSystem.h"
#include "Perception/AISense_Damage.h"


void ULimenWeaponFireMethod::ProcessFire(ALimenWeapon* Weapon, const uint64 ShotsToSimulate)
{
	check(Weapon->HasAuthority())
	check(Weapon != nullptr)
	check(ShotsToSimulate > 0)
}

bool ULimenWeaponFireMethod::IsSupportedForNetworking() const
{
	// Super::IsSupportedForNetworking();
	return true;
}

int32 ULimenWeaponFireMethod::GetFunctionCallspace(UFunction* Function, FFrame* Stack)
{
	// return Super::GetFunctionCallspace(Function, Stack);

	if (Function->FunctionFlags & FUNC_Static || !(Function->FunctionFlags & FUNC_Net))
	{
		return FunctionCallspace::Local;
	}

	bool bIsOnServer = false;
	if (const AActor* Owner = Cast<AActor>(GetOuter()))
	{
		if (const UNetDriver* NetDriver = Owner->GetNetDriver())
		{
			bIsOnServer = NetDriver->IsServer();
		}
	}

	// get the top most function
	while (Function->GetSuperFunction() != nullptr)
	{
		Function = Function->GetSuperFunction();
	}

	// Multicast RPCs
	if ((Function->FunctionFlags & FUNC_NetMulticast))
	{
		if (bIsOnServer)
		{
			// Server should execute locally and call remotely
			return (FunctionCallspace::Local | FunctionCallspace::Remote);
		}
		else
		{
			return FunctionCallspace::Local;
		}
	}

	// if we are the authority
	if (bIsOnServer)
	{
		if (Function->FunctionFlags & FUNC_NetClient)
		{
			return FunctionCallspace::Remote;
		}
		else
		{
			return FunctionCallspace::Local;
		}

	}
	// if we are not the authority
	else
	{
		if (Function->FunctionFlags & FUNC_NetServer)
		{
			return FunctionCallspace::Remote;
		}
		else
		{
			// don't replicate
			return FunctionCallspace::Local;
		}
	}
}

bool ULimenWeaponFireMethod::CallRemoteFunction(UFunction* Function, void* Parameters, FOutParmRec* OutParms,
	FFrame* Stack)
{
	// return Super::CallRemoteFunction(Function, Parameters, OutParms, Stack);

	if (!Function || !GetOuter())
	{
		return false;
	}

	AActor* Owner = Cast<AActor>(GetOuter());
	if (!Owner)
	{
		return false;
	}

	UNetDriver* NetDriver = Owner->GetNetDriver();
	if (!NetDriver)
	{
		return false;
	}

	NetDriver->ProcessRemoteFunction(Owner, Function, Parameters, OutParms, Stack, this);
	return true;
}

ULimenLineTraceFireMethod::ULimenLineTraceFireMethod()
{
	bDebugMode = false;
	TraceChannel = ECollisionChannel::ECC_Visibility;
	DecalSize = FVector(5.f);
	DecalLifetime = 10.f;
	BulletTracerSpeed = 5000.f;
	TracerLifetimeSeconds = 0.5f;
}

void ULimenLineTraceFireMethod::ProcessFire(ALimenWeapon* Weapon, const uint64 ShotsToSimulate)
{
	Super::ProcessFire(Weapon, ShotsToSimulate);

	if (!Weapon) return;

	check(Weapon->GetOwner() != nullptr) // If a weapon is firing, it should always have an owner

	FVector Start;
	FRotator Rotation;
	AActor* Owner = Weapon->GetOwner();
	if (APawn* WeaponPawnOwner = Cast<APawn>(Owner); WeaponPawnOwner && WeaponPawnOwner->GetController())
	{
		WeaponPawnOwner->GetController()->GetPlayerViewPoint(Start, Rotation);
	}
	else if (Weapon->GetOwner())
	{
		Weapon->GetOwner()->GetActorEyesViewPoint(Start,	Rotation);
	}

	const FVector End = Start + Rotation.Vector() * Weapon->GetWeaponRange();

	FCollisionQueryParams Params;
	Params.AddIgnoredSourceObject(Weapon);
	Params.AddIgnoredSourceObject(Owner);
	Params.bTraceComplex = true;
	Params.TraceTag = TEXT("WeaponBullet");

	TArray<FHitResult> OutHits;
	GetWorld()->LineTraceMultiByChannel(OutHits, Start, End, TraceChannel, Params);

	float CurrentDamageWithFalloff = Weapon->GetBaseDamage();
	uint16 DamageCount = 0;

	UAISystem* AISystem = CastChecked<UAISystem>(GetWorld()->GetAISystem());
	UAIPerceptionSystem* AIPerceptionSystem = AISystem->GetPerceptionSystem();

	TArray<AActor*> HitActors;
	HitActors.Reserve(OutHits.Num());

	FDamageParameters DamageParams;

	APawn* PawnOwner = Weapon->GetOwner<APawn>();
	const float ImpactDamageFalloffMultiplier = Weapon->GetImpactDamageFalloffMultiplier();

	for (int i = 0; i < OutHits.Num(); ++i)
	{
		// Prevent hitting the same actor twice
		AActor* ActorHit = OutHits[i].GetActor();
		if (!ActorHit || HitActors.Contains(ActorHit)) continue;

		ULimenDamageComponent* DamageComponent = ActorHit->GetComponentByClass<ULimenDamageComponent>();
		if (!DamageComponent) continue;


		float F = ImpactDamageFalloffMultiplier;
		uint64 N = ShotsToSimulate;
		if (F != 1.f)
		{
			DamageParams.DamageValue = Weapon->GetBaseDamage() * (1 - FMath::Pow(F, N)) / (1 - F);
		}
		else
		{
			DamageParams.DamageValue = Weapon->GetBaseDamage() * N;
		}

		DamageParams.HitBoneName = OutHits[i].BoneName;
		DamageParams.HitComponent = OutHits[i].Component;
		DamageParams.DamageDirection = (Start - OutHits[i].ImpactPoint).Rotation();
		DamageParams.DamageDirection.Normalize();

		TSubclassOf<ULimenDamageType> DamageTypeClass = Weapon->GetDamageType();
		if (PawnOwner)
		{
			DamageComponent->ApplyDamage(PawnOwner->GetController(), Weapon, DamageTypeClass, DamageParams);
		}
		else
		{
			DamageComponent->ApplyDamage(nullptr, Weapon, DamageTypeClass, DamageParams);
		}


		const FAIDamageEvent AIDamageEvent(ActorHit, Weapon, DamageParams.DamageValue,
			OutHits[i].ImpactPoint);

		if (AIPerceptionSystem) AIPerceptionSystem->OnEvent(AIDamageEvent);

		CurrentDamageWithFalloff *= ImpactDamageFalloffMultiplier;
		HitActors.Push(ActorHit);
		DamageCount++;
	}

	FVector FirstImpactPoint = End;
	FVector LastImpactPoint = End;

	if (!OutHits.IsEmpty())
	{
		const FHitResult& LastHit = OutHits[OutHits.Num() - 1];
		FirstImpactPoint = OutHits[0].ImpactPoint;
		LastImpactPoint = LastHit.ImpactPoint;

		if (LastHit.GetComponent()->IsA<UStaticMeshComponent>() && BulletHoleDecalMaterial)
		{
			const FRotator ImpactSurfaceOrientation = LastHit.ImpactNormal.Rotation();
			Multicast_SpawnBulletDecal(LastImpactPoint, ImpactSurfaceOrientation);
		}
	}

	if (BulletTracer)
	{
		Multicast_SpawnBulletTracer(Weapon, FirstImpactPoint);
	}


	if (bDebugMode)
	{
		DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 3.f);
		LIMEN_LOG(LogLimen, Log, this, TEXT("Hit detected: Hit %d objects, %d of them could take damage"), OutHits.Num(), DamageCount);
	}
}

void ULimenLineTraceFireMethod::Tick(const float DeltaTime)
{
	for (int32 i = Tracers.Num() - 1; i >= 0; --i)
	{
		auto& [Tracer, Lifetime, Direction] = Tracers[i];

		if (Lifetime == 0.f)
		{
			Lifetime += DeltaTime;
			continue;
		}

		const FVector NewLocation = Tracer->GetComponentLocation() + Direction * BulletTracerSpeed * DeltaTime;
		Tracer->SetWorldLocation(NewLocation);
    	
		Lifetime += DeltaTime;
		if (Lifetime >= TracerLifetimeSeconds)
		{
			Tracer->DestroyComponent();
			Tracer = nullptr;
			Tracers.RemoveAt(i, EAllowShrinking::No);
		}
	}
}

ETickableTickType ULimenLineTraceFireMethod::GetTickableTickType() const
{
	return ETickableTickType::Conditional;
}

bool ULimenLineTraceFireMethod::IsTickable() const
{
	return !HasAnyFlags(RF_ClassDefaultObject) || Tracers.IsEmpty();
}

TStatId ULimenLineTraceFireMethod::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(ULimenLineTraceFireMethod, STATGROUP_Tickables);
}

bool ULimenLineTraceFireMethod::IsTickableWhenPaused() const
{
	return false;
}

bool ULimenLineTraceFireMethod::IsTickableInEditor() const
{
	return false;
}

UWorld* ULimenLineTraceFireMethod::GetTickableGameObjectWorld() const
{
	return GetWorld();
}

void ULimenLineTraceFireMethod::Multicast_SpawnBulletTracer_Implementation(const ALimenWeapon* Weapon, const FVector& End)
{
	if (!BulletTracer) return;

	const FVector Start = Weapon->GetFiringLocation();

	FTracerData Data;
	Data.Tracer = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(),BulletTracer.Get(),
																			Start, FRotator::ZeroRotator,
																			FVector::OneVector, false);
	if (!Data.Tracer) return;

	Data.Direction = (End - Start).GetSafeNormal();
	Data.Tracer->SetWorldRotation(Data.Direction.Rotation());
	Tracers.Push(Data);
}

void ULimenLineTraceFireMethod::Multicast_SpawnBulletDecal_Implementation(const FVector& Location,
																		  const FRotator& Orientation)
{
	UDecalComponent* Decal = UGameplayStatics::SpawnDecalAtLocation(GetWorld(),
				BulletHoleDecalMaterial.Get(), DecalSize, Location, Orientation,
				DecalLifetime);
	if (Decal)
	{
		Decal->SetFadeScreenSize(0.001);
	}
}
