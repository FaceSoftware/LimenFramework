// Copyright Face Software. All Rights Reserved.


#include "Actors/Weapons/LimenLineTraceWeapon.h"

#include "CollisionQueryParams.h"
#include "DrawDebugHelpers.h"
#include "Components/LimenDamageComponent.h"
#include "Engine/HitResult.h"
#include "Engine/World.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"
#include "Kismet/KismetSystemLibrary.h"
#include "LogMacros/LimenLogMacros.h"
#include "Perception/AIPerceptionSystem.h"
#include "Perception/AISense_Damage.h"


ALimenLineTraceWeapon::ALimenLineTraceWeapon() : Super()
{
	PrimaryActorTick.bCanEverTick = true;
	bDebugMode = false;
	TraceChannel = ECollisionChannel::ECC_Visibility;
}

void ALimenLineTraceWeapon::PickUp(AController* InController, APawn* InPawn)
{
	Super::PickUp(InController, InPawn);
	CachedOwnerPawn = GetOwner<APawn>();
}

void ALimenLineTraceWeapon::FireMethod()
{
	Super::FireMethod();

	TArray<FHitResult> OutHits;
	
	FVector Start;
	FRotator Rotation;

	check(CachedOwnerPawn != nullptr) // If a weapon is being fired it should always have an owner
	CachedOwnerPawn->GetController()->GetPlayerViewPoint(Start, Rotation);

	const FVector End = Start + Rotation.Vector() * WeaponRange;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(GetOwner());
	
#if !(UE_BUILD_TEST || UE_BUILD_SHIPPING)
	Params.bDebugQuery = bDebugMode;
#endif
	
	GetWorld()->LineTraceMultiByChannel(OutHits, Start, End, TraceChannel, Params);

	float CurrentDamageWithFalloff = GetBaseDamage();
	uint16 DamageCount = 0;

	UAISystem* AISystem = CastChecked<UAISystem>(GetWorld()->GetAISystem());
	UAIPerceptionSystem* AIPerceptionSystem = AISystem->GetPerceptionSystem();

	TArray<AActor*> HitActors;
	HitActors.Reserve(OutHits.Num());

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

		FDamageParameters DamageParams;
		DamageParams.DamageValue = CurrentDamageWithFalloff;
		DamageParams.HitBoneName = OutHits[i].BoneName;
		DamageParams.HitComponent = OutHits[i].Component;

		DamageComponent->ApplyDamage(CachedOwnerPawn->GetController(), CachedOwnerPawn.Get(),
			DamageType, DamageParams);


		const FAIDamageEvent AIDamageEvent(OutHits[i].GetActor(), this, CurrentDamageWithFalloff,
			GetActorLocation());
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
