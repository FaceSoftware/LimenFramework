// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "LimenWeaponFireMethod.generated.h"

class UMaterialInterface;
enum ECollisionChannel : int;
class ALimenWeapon;


/**
 * Blueprintable and network-aware base class for weapon fire methods in the LimenWeapons system.
 * This class provides a blueprint for implementing various weapon firing behaviors and supports
 * customization and extension for specific use cases.
 */
UCLASS(Blueprintable, BlueprintType)
class LIMENWEAPONS_API ULimenWeaponFireMethod : public UObject
{
	GENERATED_BODY()

public:
	virtual void ProcessFire(ALimenWeapon* Weapon);
	virtual bool IsSupportedForNetworking() const override;
	virtual int32 GetFunctionCallspace(UFunction* Function, FFrame* Stack) override;
	virtual bool CallRemoteFunction(UFunction* Function, void* Parameters, FOutParmRec* OutParms, FFrame* Stack) override;
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

private:
	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_SpawnBulletDecal(const FVector& Location, const FRotator& Orientation);
};
