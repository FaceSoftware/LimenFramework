// Copyright Face Software. All Rights Reserved.


#include "Components/LimenVelocityBasedCameraShakeComponent.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Shakes/PerlinNoiseCameraShakePattern.h"
#include "Shakes/WaveOscillatorCameraShakePattern.h"


ULimenVelocityBasedCameraShakeComponent::ULimenVelocityBasedCameraShakeComponent()	
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 1 / 128;
	PrimaryComponentTick.bTickEvenWhenPaused = false;
	bAllowConcurrentTick = true;
	bAutoActivate = true;
	
	VelocityPtr = nullptr;
	CameraShakeClass = nullptr;
	CameraShake = nullptr;
	MovementComponent.Reset();
	MaxSpeed = 0;

	MaxLocationAmplitude = 0;
	MaxRotationAmplitude = 0;
	MaxLocationFrequency = 0;
	MaxRotationFrequency = 0;
}

void ULimenVelocityBasedCameraShakeComponent::BeginPlay()
{
	Super::BeginPlay();

	if (bAutoActivate)
	{
		const APawn* OwnerPawn = Cast<APawn>(GetOwner());
		const APlayerController* PlayerController = Cast<APlayerController>(OwnerPawn->GetController());
		if (PlayerController && !CameraShakeClass.IsNull())
		{
			CameraShake = PlayerController->PlayerCameraManager->StartCameraShake(CameraShakeClass.LoadSynchronous(), 1.f, ECameraShakePlaySpace::CameraLocal);
			PerlinNoisePattern = Cast<UPerlinNoiseCameraShakePattern>(CameraShake->GetRootShakePattern());
			WaveOscillatorPattern = Cast<UWaveOscillatorCameraShakePattern>(CameraShake->GetRootShakePattern());
		}
	}
	
	MovementComponent = GetOwner()->GetComponentByClass<UPawnMovementComponent>();
	VelocityPtr = &MovementComponent->Velocity;
}

void ULimenVelocityBasedCameraShakeComponent::TickComponent(float DeltaTime, ELevelTick TickType,
															FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (CameraShake != nullptr && IsActive())
	{
		// If the speed is 0, set the scale to 1, so it doesn't completely stop
		if (PerlinNoisePattern != nullptr)
		{
			PerlinNoisePattern->LocationAmplitudeMultiplier = GetSpeed() * MaxLocationAmplitude / MaxSpeed + 1;
			PerlinNoisePattern->RotationAmplitudeMultiplier = GetSpeed() * MaxRotationAmplitude / MaxSpeed + 1;

			PerlinNoisePattern->RotationFrequencyMultiplier = GetSpeed() * MaxLocationFrequency / MaxSpeed + 1;
			PerlinNoisePattern->LocationFrequencyMultiplier = GetSpeed() * MaxRotationFrequency / MaxSpeed + 1;
		}
		else if (WaveOscillatorPattern != nullptr)
		{
			WaveOscillatorPattern->LocationAmplitudeMultiplier = GetSpeed() * MaxLocationAmplitude / MaxSpeed + 1;
			WaveOscillatorPattern->RotationAmplitudeMultiplier = GetSpeed() * MaxRotationAmplitude / MaxSpeed + 1;

			WaveOscillatorPattern->RotationFrequencyMultiplier = GetSpeed() * MaxLocationFrequency / MaxSpeed + 1;
			WaveOscillatorPattern->LocationFrequencyMultiplier = GetSpeed() * MaxRotationFrequency / MaxSpeed + 1;
		}
	}
}

void ULimenVelocityBasedCameraShakeComponent::BeginDestroy()
{
	Super::BeginDestroy();

	VelocityPtr = nullptr;
}

void ULimenVelocityBasedCameraShakeComponent::Activate(bool bReset)
{
	Super::Activate(bReset);

	const APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (const APlayerController* PlayerController = Cast<APlayerController>(OwnerPawn->GetController()))
	{
		CameraShake = PlayerController->PlayerCameraManager->StartCameraShake(CameraShakeClass.LoadSynchronous(), 1.f, ECameraShakePlaySpace::CameraLocal);
		PerlinNoisePattern = Cast<UPerlinNoiseCameraShakePattern>(CameraShake->GetRootShakePattern());
		WaveOscillatorPattern = Cast<UWaveOscillatorCameraShakePattern>(CameraShake->GetRootShakePattern());
	}
}

void ULimenVelocityBasedCameraShakeComponent::Deactivate()
{
	Super::Deactivate();

	if (CameraShake && CameraShake->IsActive())
	{
		CameraShake->StopShake();
	}
}

float ULimenVelocityBasedCameraShakeComponent::GetSpeed() const
{
	if (VelocityPtr == nullptr)
	{
		return NAN;
	}

	return VelocityPtr->Length();
}
