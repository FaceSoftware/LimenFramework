// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tickable.h"
#include "UObject/Object.h"
#include "LimenWeaponFireMethod.generated.h"

struct FVector_NetQuantize100;
class UNiagaraComponent;
class UNiagaraSystem;
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
	virtual void ProcessFire(ALimenWeapon* Weapon, const uint64 ShotsToSimulate);
	virtual bool IsSupportedForNetworking() const override;
	virtual int32 GetFunctionCallspace(UFunction* Function, FFrame* Stack) override;
	virtual bool CallRemoteFunction(UFunction* Function, void* Parameters, FOutParmRec* OutParms, FFrame* Stack) override;
};


UCLASS()
class ULimenLineTraceFireMethod : public ULimenWeaponFireMethod, public FTickableGameObject
{
	GENERATED_BODY()

public:
	ULimenLineTraceFireMethod();

	virtual void ProcessFire(ALimenWeapon* Weapon, const uint64 ShotsToSimulate) override;

#pragma region FTickableGameObject

	virtual void Tick(float DeltaTime) override;
	virtual ETickableTickType GetTickableTickType() const override;
	virtual bool IsTickable() const override;
	virtual TStatId GetStatId() const override;
	virtual bool IsTickableWhenPaused() const override;
	virtual bool IsTickableInEditor() const override;
	virtual UWorld* GetTickableGameObjectWorld() const override;

#pragma endregion

protected:
	UPROPERTY(EditDefaultsOnly, Category="Debug")
	bool bDebugMode;
	
	UPROPERTY(EditDefaultsOnly, Category="Collision")
	TEnumAsByte<ECollisionChannel> TraceChannel;

	UPROPERTY(EditDefaultsOnly, Category="VFX")
	TObjectPtr<UMaterialInterface> BulletHoleDecalMaterial;
	UPROPERTY(EditDefaultsOnly, Category="VFX")
	TObjectPtr<UNiagaraSystem> BulletTracer;
	UPROPERTY(EditDefaultsOnly, Category="VFX")
	float BulletTracerSpeed;
	UPROPERTY(EditDefaultsOnly, Category="VFX")
	float TracerLifetimeSeconds;
	UPROPERTY(EditDefaultsOnly, Category="VFX")
	FVector DecalSize;
	UPROPERTY(EditDefaultsOnly, Category="VFX", meta=(Units=seconds))
	float DecalLifetime;

private:
	struct FTracerData
	{
		UNiagaraComponent* Tracer = nullptr;
		float Lifetime = 0.f;
		FVector Direction = FVector::ZeroVector;
	};
	
	TArray<FTracerData> Tracers;
	
	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_SpawnBulletTracer(const ALimenWeapon* Weapon, const FVector& End);
	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_SpawnBulletDecal(const FVector& Location, const FRotator& Orientation);
};
