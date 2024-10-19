// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LimenVelocityBasedCameraShakeComponent.generated.h"


class UWaveOscillatorCameraShakePattern;
class UPerlinNoiseCameraShakePattern;
/**
 * 
 */
UCLASS()
class LIMENLOCOMOTION_API ULimenVelocityBasedCameraShakeComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	ULimenVelocityBasedCameraShakeComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void BeginDestroy() override;

	virtual void Activate(bool bReset) override;
	virtual void Deactivate() override;

protected:
	UPROPERTY(EditAnywhere, Category="Limen")
	float MaxSpeed;
	UPROPERTY(EditAnywhere, Category="Limen")
	float MaxLocationAmplitude;
	UPROPERTY(EditAnywhere, Category="Limen")
	float MaxRotationAmplitude;
	UPROPERTY(EditAnywhere, Category="Limen")
	float MaxLocationFrequency;
	UPROPERTY(EditAnywhere, Category="Limen")
	float MaxRotationFrequency;
	UPROPERTY(EditAnywhere, Category="Limen")
	TSoftClassPtr<UCameraShakeBase> CameraShakeClass;
	UPROPERTY()
	TObjectPtr<UCameraShakeBase> CameraShake;
	
	float GetSpeed() const;

private:
	UPROPERTY()
	TObjectPtr<UPerlinNoiseCameraShakePattern> PerlinNoisePattern;
	UPROPERTY()
	TObjectPtr<UWaveOscillatorCameraShakePattern> WaveOscillatorPattern;
	UPROPERTY()
	TWeakObjectPtr<UPawnMovementComponent> MovementComponent;
	const FVector* VelocityPtr;
};
