// Copyright Face Software. All Rights Reserved.


#include "Components/LimenMovementComponent.h"

#include "BlueprintLibraries/LimenCoreStatics.h"
#include "Serialization/MemoryReader.h"


ULimenMovementComponent::ULimenMovementComponent(const FObjectInitializer& InObjectInitializer)
	: Super(InObjectInitializer)
{	
	SlowWalkSpeed = 100.f;
	MaxWalkSpeed = 400.f;
	SprintSpeed = 800.f;
	bSprintingEnabledByDefault = false;

	CrouchSlowWalkSpeed = 50.f;
	MaxWalkSpeedCrouched = 200.f;
	CrouchSprintSpeed = 400.f;
	bSlowWalkEnabledByDefault = false;

	bSprintingEnabledByDefault = false;
	WalkMode = EWalkModifier::None;
	bEnableAirStrafing = true;
	AirAcceleration = 10.f;
	MaxAirStrafeSpeed = 30.f;
	bAllowJumpingWhileCrouched = true;
	bLogCurrentSpeed = false;
	bEnableSurfing = false;
}

void ULimenMovementComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void ULimenMovementComponent::BeginPlay()
{
	MaxWalkSpeedCrouched_Cached = MaxWalkSpeedCrouched;
	MaxWalkSpeed_Cached = MaxWalkSpeed;

	SetupAirStrafing();
	const EWalkModifier InitialWalkModifier = bSprintingEnabledByDefault ? EWalkModifier::Sprint : ( bSlowWalkEnabledByDefault ? EWalkModifier::SlowWalk : EWalkModifier::None );
	SetWalkModifier(InitialWalkModifier);

	Super::BeginPlay();
}

void ULimenMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (bLogCurrentSpeed && GetOwner()->HasAuthority())
	{
		LimenLog(this, FString::Printf(TEXT("Speed: %f"), Velocity.Size2D()), ELogType::Log,
				 true, FName(TEXT("Speed")));
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

void ULimenMovementComponent::SetSprintMode(const bool bEnabled)
{	
	SetWalkModifier(bEnabled ? EWalkModifier::Sprint : EWalkModifier::None);
}

bool ULimenMovementComponent::IsSprinting() const
{
	return IsWalking() && WalkMode == EWalkModifier::Sprint;
}

void ULimenMovementComponent::SetSlowWalkMode(const bool bEnabled)
{
	SetWalkModifier(bEnabled ? EWalkModifier::SlowWalk : EWalkModifier::None);
}

bool ULimenMovementComponent::IsSlowWalking() const
{
	return IsMovingOnGround() && WalkMode == EWalkModifier::SlowWalk;
}

void ULimenMovementComponent::SetWalkModifier(const EWalkModifier NewModifier)
{
	if (WalkMode == NewModifier) return;

	WalkMode = NewModifier;
	OnWalkModeChanged(WalkMode);
}

ULimenMovementComponent::EWalkModifier ULimenMovementComponent::GetWalkModifier() const
{
	return WalkMode;
}

void ULimenMovementComponent::SetMaxSprintSpeed(const float Multiplier)
{
	SprintSpeed = Multiplier;
}

float ULimenMovementComponent::GetMaxSprintSpeed() const
{
	return SprintSpeed;
}

void ULimenMovementComponent::SetCrouchSprintSpeed(const float Multiplier)
{
	CrouchSprintSpeed = Multiplier;
}

float ULimenMovementComponent::GetCrouchSprintSpeed() const
{
	return CrouchSprintSpeed;
}

void ULimenMovementComponent::UpdateFromCompressedFlags(const uint8 Flags)
{
	Super::UpdateFromCompressedFlags(Flags);

	const bool bSprintEnabled = (Flags & FSavedMove_Character::FLAG_Custom_0) != 0;
	const bool bSlowWalkEnabled = (Flags & FSavedMove_Character::FLAG_Custom_1) != 0;

	if (bSprintEnabled)
	{
		SetWalkModifier(EWalkModifier::Sprint);
	}
	else if (bSlowWalkEnabled)
	{
		SetWalkModifier(EWalkModifier::SlowWalk);
	}
	else
	{
		SetWalkModifier(EWalkModifier::None);
	}
}

void ULimenMovementComponent::PhysFalling(const float DeltaTime, const int32 Iterations)
{
	Super::PhysFalling(DeltaTime, Iterations);
	PhysAirStrafing(DeltaTime, Iterations);
	PhysSurfing(DeltaTime, Iterations);
}

void ULimenMovementComponent::PhysAirStrafing(const float DeltaTime, const int32 Iterations)
{
	if (!bEnableAirStrafing) return;
	if (Iterations >= MaxSimulationIterations) return;
	

	LimenLog(this, FString::Printf(TEXT("Acceleration: %s"), *Acceleration.ToString()), ELogType::Log,
		 true, FName(TEXT("Acceleration")));

	const FVector WishVelocity = Acceleration.GetSafeNormal();
	const float WishSpeed = FMath::Min(Acceleration.Length(), MaxAirStrafeSpeed);

	const float CurrentSpeed = FVector::DotProduct(Velocity, WishVelocity);
	const float AddSpeed = WishSpeed - CurrentSpeed;
	if (AddSpeed <= 0.f) return;

	float AccelSpeed = Acceleration.Length() * AirAcceleration * DeltaTime;
	AccelSpeed = FMath::Min(AccelSpeed, AddSpeed);

	const FVector HorizontalWishVelocity(WishVelocity.X, WishVelocity.Y, 0.f);
	Velocity += AccelSpeed * HorizontalWishVelocity;
}

void ULimenMovementComponent::PhysSurfing(float DeltaTime, const int32 Iterations)
{
	if (!bEnableAirStrafing) return;
	if (Iterations >= MaxSimulationIterations) return;

	
}

bool ULimenMovementComponent::CanAttemptJump() const
{
	if (!bAllowJumpingWhileCrouched) return Super::CanAttemptJump();

	// Falling included for double-jump and non-zero jump hold time,
	// but validated by character.
	return IsJumpAllowed() && (IsMovingOnGround() || IsFalling());
}

FVector ULimenMovementComponent::ComputeSlideVector(const FVector& Delta, const float Time, const FVector& Normal,
	const FHitResult& Hit) const
{
	return bEnableSurfing
		? Super::ComputeSlideVector(Delta, Time, Normal, Hit)
		: UPawnMovementComponent::ComputeSlideVector(Delta, Time, Normal, Hit);
}

FVector ULimenMovementComponent::HandleSlopeBoosting(const FVector& SlideResult, const FVector& Delta, const float Time,
                                                     const FVector& Normal, const FHitResult& Hit) const
{
	if (bEnableSurfing) { checkNoEntry() }
	return Super::HandleSlopeBoosting(SlideResult, Delta, Time, Normal, Hit);
}

void ULimenMovementComponent::OnWalkModeChanged(const EWalkModifier NewMode)
{
	check(NewMode == WalkMode)

	switch (NewMode)
	{
	case EWalkModifier::None:
		{
			MaxWalkSpeedCrouched = MaxWalkSpeedCrouched_Cached;
			MaxWalkSpeed = MaxWalkSpeed_Cached;
		}
		break;

	case EWalkModifier::SlowWalk:
		{
			MaxWalkSpeedCrouched = CrouchSlowWalkSpeed;
			MaxWalkSpeed = SlowWalkSpeed;
		}
		break;

	case EWalkModifier::Sprint:
		{
			MaxWalkSpeedCrouched = CrouchSprintSpeed;
			MaxWalkSpeed = SprintSpeed;
		}
		break;
	}

	OnWalkModifierChanged.Broadcast(NewMode);
}

void ULimenMovementComponent::SetupAirStrafing()
{
	if (!bEnableAirStrafing) return;
	AirControl = 0.f;
	AirControlBoostMultiplier = 0.f;
	AirControlBoostVelocityThreshold = 0.f;
	BrakingDecelerationFalling = 0.f;
}
