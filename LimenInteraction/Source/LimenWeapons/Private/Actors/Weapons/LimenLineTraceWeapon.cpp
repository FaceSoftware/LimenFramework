// Copyright Face Software. All Rights Reserved.


#include "Actors/Weapons/LimenLineTraceWeapon.h"

#include "Interfaces/LimenDamageable.h"
#include "LogMacros/LimenLogMacros.h"


ALimenLineTraceWeapon::ALimenLineTraceWeapon(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	bDebugMode = false;
	TraceChannel = ECollisionChannel::ECC_Camera;
}

void ALimenLineTraceWeapon::FireMethod()
{
	Super::FireMethod();

	TArray<FHitResult> OutHits;
	
	FVector Start;
	FRotator Rotation;
	if (GetOwnerPawn() == nullptr)
	{
		return;
	}

	AController* OwnerController = GetOwnerPawn()->GetController();
	if (OwnerController == nullptr)
	{
		return;
	}
	
	OwnerController->GetPlayerViewPoint(Start, Rotation);
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
			Damageable->ApplyPointDamage(OwnerController, this, CurrentDamageWithFalloff);
			DamageCount++;
		}

		CurrentDamageWithFalloff *= ImpactDamageFalloffMultiplier;
	}
	
	OnWeaponFired.Broadcast(OwnerController, this, OutHits);

	LIMEN_LOG(LogLimen, Log, this, "Hit detected: Hit %d objects, %d of them could take damage", OutHits.Num(), DamageCount);

	if (bDebugMode)
	{
		DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 3.f);
	}
}
