// Copyright Face Software. All Rights Reserved.


#include "Components/LimenMovementComponent.h"


ULimenMovementComponent::ULimenMovementComponent(const FObjectInitializer& InObjectInitializer)
	: Super(InObjectInitializer)
{
	FastWalkSpeedMultiplier = 2.5f;
	CrouchFastWalkSpeedMultiplier = 2.5f;
	bFastMovementEnabledByDefault = false;
	bIsFastMovementEnabled = false;
	bEnableAirStrafing = true;
	AirAcceleration = 10.f;
	MaxAirStrafeSpeed = 30.f;
	bAllowJumpingWhileCrouched = true;
	bLogCurrentSpeed = false;
}

void ULimenMovementComponent::BeginPlay()
{
	SetupAirStrafing();
	SetFastMovement(bFastMovementEnabledByDefault);

	Super::BeginPlay();
}

void ULimenMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (bLogCurrentSpeed)
	{
		GEngine->AddOnScreenDebugMessage(FName(TEXT("Speed")).ToUnstableInt(), 0.F, FColor::Green,
			FString::Printf(TEXT("Speed: %f"), Velocity.Size2D()));
		
	}
}

FNetworkPredictionData_Client* ULimenMovementComponent::GetPredictionData_Client() const
{
	if (!ClientPredictionData)
	{
		ULimenMovementComponent* MutableThis = const_cast<ULimenMovementComponent*>(this);
		MutableThis->ClientPredictionData = new FNetworkPredictionData_Client_Limen(*this);
	}

	return ClientPredictionData;
}

void ULimenMovementComponent::SetFastMovement(const bool bEnabled)
{	
	if (bEnabled == bIsFastMovementEnabled)
	{
		return;
	}

	bIsFastMovementEnabled = bEnabled;
	OnFastMovementChanged();
}

bool ULimenMovementComponent::IsFastMovementEnabled() const
{
	return bIsFastMovementEnabled;
}

void ULimenMovementComponent::UpdateFromCompressedFlags(uint8 Flags)
{
	Super::UpdateFromCompressedFlags(Flags);

	const bool bFastMovementEnabled = (Flags & FSavedMove_Character::FLAG_Custom_0) != 0;
	SetFastMovement(bFastMovementEnabled); // Re-apply local state for prediction
}

void ULimenMovementComponent::PhysFalling(const float DeltaTime, const int32 Iterations)
{
	Super::PhysFalling(DeltaTime, Iterations);
	PhysAirStrafing(DeltaTime);
}

void ULimenMovementComponent::PhysAirStrafing(const float DeltaTime)
{
	if (!bEnableAirStrafing) return;

	const FVector WishVelocity = GetLastInputVector().GetSafeNormal();
	const float WishSpeed =
		FMath::Min((WishVelocity * MaxWalkSpeed).Length(), MaxAirStrafeSpeed);

	const float CurrentSpeed = FVector::DotProduct(Velocity, WishVelocity);
	const float AddSpeed = WishSpeed - CurrentSpeed;
	if (AddSpeed <= 0.f) return;

	float AccelSpeed = AirAcceleration * MaxWalkSpeed * DeltaTime;
	AccelSpeed = FMath::Min(AccelSpeed, AddSpeed);

	Velocity += AccelSpeed * WishVelocity;
}

bool ULimenMovementComponent::CanAttemptJump() const
{
	if (!bAllowJumpingWhileCrouched) return Super::CanAttemptJump();

	// Falling included for double-jump and non-zero jump hold time,
	// but validated by character.
	return IsJumpAllowed() && (IsMovingOnGround() || IsFalling());
}

void ULimenMovementComponent::OnFastMovementChanged()
{
	if (bIsFastMovementEnabled)
	{
		MaxWalkSpeedCrouched *= CrouchFastWalkSpeedMultiplier;
		MaxWalkSpeed *= FastWalkSpeedMultiplier;
	}
	else
	{
		MaxWalkSpeedCrouched /= CrouchFastWalkSpeedMultiplier;
		MaxWalkSpeed /= FastWalkSpeedMultiplier;
	}
}

void ULimenMovementComponent::SetupAirStrafing()
{
	if (!bEnableAirStrafing) return;
	AirControl = 0.f;
	AirControlBoostMultiplier = 0.f;
	AirControlBoostVelocityThreshold = 0.f;
	BrakingDecelerationFalling = 0.f;
}
