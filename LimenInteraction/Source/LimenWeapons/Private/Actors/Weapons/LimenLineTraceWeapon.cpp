// Copyright Face Software. All Rights Reserved.


#include "Actors/Weapons/LimenLineTraceWeapon.h"

#include "Interfaces/LimenDamageable.h"
#include "Kismet/KismetSystemLibrary.h"
#include "LogMacros/LimenInteractionLogMacros.h"
#include "LogMacros/LimenLogMacros.h"

ALimenLineTraceWeapon::ALimenLineTraceWeapon() : Super()
{
	PrimaryActorTick.bCanEverTick = true;
	bDebugMode = false;
}

void ALimenLineTraceWeapon::FireMethod()
{
	Super::FireMethod();

	TArray<FHitResult> OutHits;
	
	FVector Start;
	FRotator Rotation;
	GetOwningController()->GetPlayerViewPoint(Start, Rotation);
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
	for (int i = 0; i < OutHits.Num(); ++i)
	{
		// C++ Interface
		if (auto* Damageable = Cast<ILimenDamageable>(OutHits[i].GetActor()))
		{
			Damageable->ApplyPointDamage(GetOwningController(), GetOwningPawn(), CurrentDamageWithFalloff);
			DamageCount++;
		}

		CurrentDamageWithFalloff *= ImpactDamageFalloffMultiplier;
	}

	LIMEN_LOG(LogLimen, Log, this, "Hit detected: Hit %d objects, %d of them could take damage", OutHits.Num(), DamageCount);

	if (bDebugMode)
	{
		DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 3.f);
	}
}
