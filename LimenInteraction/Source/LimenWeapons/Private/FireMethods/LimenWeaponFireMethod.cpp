// Fill out your copyright notice in the Description page of Project Settings.


#include "FireMethods/LimenWeaponFireMethod.h"

#include "Components/LimenDamageComponent.h"
#include "DamageTypes/LimenDamageType.h"
#include "AISystem.h"
#include "CollisionQueryParams.h"
#include "DrawDebugHelpers.h"
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


void ULimenWeaponFireMethod::ProcessFire(ALimenWeapon* Weapon)
{
	check(Weapon->HasAuthority())
	check(Weapon != nullptr)
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
}

void ULimenLineTraceFireMethod::ProcessFire(ALimenWeapon* Weapon)
{
	Super::ProcessFire(Weapon);

	FVector Start;
	FRotator Rotation;

	check(Weapon->GetOwner() != nullptr) // If a weapon is firing, it should always have an owner
	AActor* WeaponOwner = Weapon->GetOwner();
	WeaponOwner->GetActorEyesViewPoint(Start, Rotation);

	const FVector End = Start + Rotation.Vector() * Weapon->GetWeaponRange();

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Weapon);
	Params.AddIgnoredActor(WeaponOwner);
	Params.bTraceComplex = true;
	Params.TraceTag = TEXT("Weapon bullet");
	
#if !(UE_BUILD_TEST || UE_BUILD_SHIPPING)
	Params.bDebugQuery = bDebugMode;
#endif

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
		if (!OutHits[i].GetActor() || HitActors.Contains(OutHits[i].GetActor()))
		{
			continue;
		}
		ULimenDamageComponent* DamageComponent = OutHits[i].GetActor()->GetComponentByClass<ULimenDamageComponent>();
		if (!DamageComponent)
		{
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

	if (!OutHits.IsEmpty())
	{
		if (const FHitResult& LastHit = OutHits[OutHits.Num() - 1];
			LastHit.GetComponent()->IsA<UStaticMeshComponent>() && BulletHoleDecalMaterial)
		{
			const FVector ImpactPoint = LastHit.ImpactPoint;
			const FRotator ImpactSurfaceOrientation = LastHit.ImpactNormal.Rotation();
			Multicast_SpawnBulletDecal(ImpactPoint, ImpactSurfaceOrientation);

			// Decal->AttachToComponent(LastHit.GetComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		}
	}

	LIMEN_LOG(LogLimen, Log, this, "Hit detected: Hit %d objects, %d of them could take damage", OutHits.Num(), DamageCount);

	if (bDebugMode)
	{
		DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 3.f);
	}
}

void ULimenLineTraceFireMethod::Multicast_SpawnBulletDecal_Implementation(const FVector& Location, const FRotator& Orientation)
{
	UDecalComponent* Decal = UGameplayStatics::SpawnDecalAtLocation(GetWorld(),
				BulletHoleDecalMaterial.Get(), DecalSize, Location, Orientation,
				DecalLifetime);
	if (Decal)
	{
		Decal->SetFadeScreenSize(0.001);
	}
}
