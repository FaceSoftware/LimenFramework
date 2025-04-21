// Fill out your copyright notice in the Description page of Project Settings.


#include "FireMethods/LimenWeaponFireMethod.h"

#include "Components/LimenDamageComponent.h"
#include "DamageTypes/LimenDamageType.h"
#include "AISystem.h"
#include "CollisionQueryParams.h"
#include "DrawDebugHelpers.h"
#include "Actors/LimenWeapon.h"
#include "Engine/EngineTypes.h"
#include "Engine/HitResult.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "LogMacros/LimenLogMacros.h"
#include "Perception/AIPerceptionSystem.h"
#include "Perception/AISense_Damage.h"


void ULimenWeaponFireMethod::ProcessFire(ALimenWeapon* Weapon)
{
	check(Weapon != nullptr)
}

ULimenLineTraceFireMethod::ULimenLineTraceFireMethod()
{
	bDebugMode = false;
	TraceChannel = ECollisionChannel::ECC_Visibility;
}

void ULimenLineTraceFireMethod::ProcessFire(ALimenWeapon* Weapon)
{
	Super::ProcessFire(Weapon);

	TArray<FHitResult> OutHits;
	
	FVector Start;
	FRotator Rotation;

	check(Weapon->GetOwner() != nullptr) // If a weapon is firing, it should always have an owner
	AActor* WeaponOwner = Weapon->GetOwner();
	WeaponOwner->GetActorEyesViewPoint(Start, Rotation);

	const FVector End = Start + Rotation.Vector() * Weapon->GetWeaponRange();

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Weapon);
	Params.AddIgnoredActor(WeaponOwner);
	
#if !(UE_BUILD_TEST || UE_BUILD_SHIPPING)
	Params.bDebugQuery = bDebugMode;
#endif

	if (!GetWorld()->LineTraceMultiByChannel(OutHits, Start, End, TraceChannel, Params))
	{
		return;
	}

	float CurrentDamageWithFalloff = Weapon->GetBaseDamage();
	uint16 DamageCount = 0;

	UAISystem* AISystem = CastChecked<UAISystem>(GetWorld()->GetAISystem());
	UAIPerceptionSystem* AIPerceptionSystem = AISystem->GetPerceptionSystem();

	TArray<AActor*> HitActors;
	HitActors.Reserve(OutHits.Num());

	FDamageParameters DamageParams;

	APawn* PawnOwner = Weapon->GetOwner<APawn>();
	const float ImpactDamageFalloffMultiplier = Weapon->GetImpactDamageFalloffMultiplier();;;

	for (int i = 0; i < OutHits.Num(); ++i)
	{
		// Prevent hitting the same actor twice
		if (!OutHits[i].GetActor() || HitActors.Contains(OutHits[i].GetActor()))
		{
			continue;
		}
		ULimenDamageComponent* DamageComponent = OutHits[i].GetActor()->GetComponentByClass<ULimenDamageComponent>();
		if (!DamageComponent)
		{
			DamageCount++;
			continue;
		}

		DamageParams.DamageValue = CurrentDamageWithFalloff;
		DamageParams.HitBoneName = OutHits[i].BoneName;
		DamageParams.HitComponent = OutHits[i].Component;
		DamageParams.DamageDirection = OutHits[i].ImpactPoint - Start;
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


		const FAIDamageEvent AIDamageEvent(OutHits[i].GetActor(), Weapon, CurrentDamageWithFalloff,
			OutHits[i].ImpactPoint);

		if (AIPerceptionSystem) AIPerceptionSystem->OnEvent(AIDamageEvent);

		CurrentDamageWithFalloff *= ImpactDamageFalloffMultiplier;
		HitActors.Push(OutHits[i].GetActor());
		DamageCount++;
	}

	LIMEN_LOG(LogLimen, Log, this, "Hit detected: Hit %d objects, %d of them could take damage", OutHits.Num(), DamageCount);

	if (bDebugMode)
	{
		DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 3.f);
	}
}
