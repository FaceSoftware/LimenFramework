// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "LimenWeaponFireMethod.generated.h"

class UMaterialInterface;
enum ECollisionChannel : int;
class ALimenWeapon;


/**
 * ULimenWeaponFireMethod
 *
 * A base class that defines a method for handling weapon firing in the LimenWeapons system.
 * Acts as an abstract representation for managing weapon fire mechanics.
 *
 * Inherits from:
 *   - UObject: The base class for all UE objects.
 *
 * Intended Usage:
 *   This class is designed to be extended to implement specific weapon fire behaviors
 *   and mechanisms. It serves as the foundation for weapon fire-related logic.
 *
 * Module:
 *   - LIMENWEAPONS_API: Provides API access to this class for the LimenWeapons module.
 */
UCLASS(Blueprintable, BlueprintType)
class LIMENWEAPONS_API ULimenWeaponFireMethod : public UObject
{
	GENERATED_BODY()

public:
	virtual void ProcessFire(ALimenWeapon* Weapon);
};


UCLASS()
class ULimenLineTraceFireMethod : public ULimenWeaponFireMethod
{
	GENERATED_BODY()

public:
	ULimenLineTraceFireMethod();
	virtual void ProcessFire(ALimenWeapon* Weapon) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category="Debug")
	bool bDebugMode;
	
	UPROPERTY(EditDefaultsOnly, Category="Collision")
	TEnumAsByte<ECollisionChannel> TraceChannel;

	UPROPERTY(EditDefaultsOnly, Category="VFX")
	TObjectPtr<UMaterialInterface> BulletHoleDecalMaterial;
	UPROPERTY(EditDefaultsOnly, Category="VFX")
	FVector DecalSize;
	UPROPERTY(EditDefaultsOnly, Category="VFX", meta=(Units=seconds))
	float DecalLifetime;
	
};
