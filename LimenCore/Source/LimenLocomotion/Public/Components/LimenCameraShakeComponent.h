// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BlueprintLibraries/LimenCoreStatics.h"
#include "Camera/CameraShakeSourceComponent.h"
#include "LimenCameraShakeComponent.generated.h"


class UPawnMovementComponent;
class UCameraShakeBase;
class UWaveOscillatorCameraShakePattern;
class UPerlinNoiseCameraShakePattern;


/**
 * 
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LIMENLOCOMOTION_API ULimenCameraShakeComponent : public UCameraShakeSourceComponent
{
	GENERATED_BODY()

public:
	explicit ULimenCameraShakeComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void Activate(bool bReset) override;
	virtual void Deactivate() override;

protected:
	UPROPERTY(EditAnywhere, Category="Limen")
	FPair SlowestSpeedScale;
	UPROPERTY(EditAnywhere, Category="Limen")
	FPair FastestSpeedScale;

	UPROPERTY()
	TObjectPtr<UCameraShakeBase> CameraShakePtr;
	
	float GetSpeed() const;

private:
	UPROPERTY()
	TObjectPtr<UPerlinNoiseCameraShakePattern> PerlinNoisePattern;
	UPROPERTY()
	TObjectPtr<UWaveOscillatorCameraShakePattern> WaveOscillatorPattern;

	TWeakObjectPtr<UPawnMovementComponent> MovementComponent;

	const FVector* VelocityPtr;
};
