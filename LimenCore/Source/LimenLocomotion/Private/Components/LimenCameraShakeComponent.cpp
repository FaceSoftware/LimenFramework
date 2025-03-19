// Copyright Face Software. All Rights Reserved.


#include "Components/LimenCameraShakeComponent.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "Shakes/PerlinNoiseCameraShakePattern.h"
#include "Shakes/WaveOscillatorCameraShakePattern.h"


ULimenCameraShakeComponent::ULimenCameraShakeComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 1 / 64;
	PrimaryComponentTick.bTickEvenWhenPaused = false;
	bAllowConcurrentTick = true;
	bAutoActivate = true;
	
	VelocityPtr = nullptr;
	CameraShakeClass = nullptr;
	CameraShake = nullptr;
}

void ULimenCameraShakeComponent::BeginPlay()
{
	Super::BeginPlay();
	
	MovementComponent = GetOwner()->GetComponentByClass<UPawnMovementComponent>();
	VelocityPtr = &MovementComponent->Velocity;

	if (IsActive()) Activate(true);
}

void ULimenCameraShakeComponent::TickComponent(float DeltaTime, ELevelTick TickType,
															FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (const float CurrentSpeed = GetSpeed(); !FMath::IsNaN(CurrentSpeed) && CameraShake != nullptr)
	{
		float NewScale = CurrentSpeed * FastestSpeedScale.B / FastestSpeedScale.A;
		if (NewScale > FastestSpeedScale.B)
		{
			NewScale = FastestSpeedScale.B;
		}
		else if (NewScale < SlowestSpeedScale.B)
		{
			NewScale = SlowestSpeedScale.B;
		}

		FMinimalViewInfo Temp;
		CameraShake->ShakeScale = NewScale;
		CameraShake->UpdateAndApplyCameraShake(DeltaTime, 1.f, Temp);
	}
}

void ULimenCameraShakeComponent::Activate(bool bReset)
{
	Super::Activate(bReset);

	const APawn* OwnerPawn = Cast<APawn>(GetOwner());
	const APlayerController* PlayerController = Cast<APlayerController>(OwnerPawn->GetController());
	if (PlayerController == nullptr)
	{
		return;
	}

	CameraShake = PlayerController->PlayerCameraManager->StartCameraShake(CameraShakeClass.LoadSynchronous(), 1.f, ECameraShakePlaySpace::CameraLocal);
	if (CameraShake != nullptr)
	{
		PerlinNoisePattern = Cast<UPerlinNoiseCameraShakePattern>(CameraShake->GetRootShakePattern());
		WaveOscillatorPattern = Cast<UWaveOscillatorCameraShakePattern>(CameraShake->GetRootShakePattern());
	}
}

void ULimenCameraShakeComponent::Deactivate()
{
	Super::Deactivate();

	if (CameraShake && CameraShake->IsActive())
	{
		CameraShake->StopShake();
	}
}

float ULimenCameraShakeComponent::GetSpeed() const
{
	if (VelocityPtr == nullptr)
	{
		return NAN;
	}

	return VelocityPtr->Length();
}
