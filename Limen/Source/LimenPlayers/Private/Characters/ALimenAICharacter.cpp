// Copyright Face Software. All Rights Reserved.


#include "Characters/LimenAICharacter.h"

#include "AIControllers/LimenAIControllerBase.h"
#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/Interaction/LimenProximityInteractionComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"


ALimenAICharacter::ALimenAICharacter(const FObjectInitializer& InObjectInitializer) : Super(InObjectInitializer)
{	
	PrimaryActorTick.bCanEverTick = true;
	bIsFrozen = false;
	MovementModeBeforeFreeze = MOVE_None;
	AutoPossessPlayer = EAutoReceiveInput::Disabled;
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	InteractionComponent = CreateOptionalDefaultSubobject<ULimenProximityInteractionComponent>(TEXT("ProximityInteractionComponent"));
	if (InteractionComponent != nullptr)
	{
		InteractionComponent->SetAutoActivate(true);
	}
}

void ALimenAICharacter::BeginPlay()
{
	Super::BeginPlay();

	LimenAIController = Cast<ALimenAIControllerBase>(GetController());
}

void ALimenAICharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bIsFrozen)
	{
		SetActorRotation(FrozenRotation);
	}
}

void ALimenAICharacter::Freeze(const bool bFreezePose)
{
	if (bIsFrozen || GetCharacterMovement() == nullptr)
	{
		return;
	}
	MovementModeBeforeFreeze = GetCharacterMovement()->MovementMode;
	bIsFrozen = true;

	if (!bFreezePose)
	{
		return;
	}

	GetCharacterMovement()->DisableMovement();
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!IsValid(AnimInstance))
	{
		return;
	}

	AnimInstance->Montage_Pause();
	// Disable physics and tick
	GetMesh()->SetComponentTickEnabled(false);

	FrozenRotation = GetActorRotation();
}

void ALimenAICharacter::UnFreeze()
{
	if (!bIsFrozen)
	{
		return;
	}

	bIsFrozen = false;

	GetCharacterMovement()->SetMovementMode(MovementModeBeforeFreeze);
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance == nullptr)
	{
		return;
	}

	AnimInstance->Montage_Resume(nullptr);
	// Disable physics and tick
	GetMesh()->SetComponentTickEnabled(true);
}

bool ALimenAICharacter::IsFrozen() const
{
	return bIsFrozen;
}

bool ALimenAICharacter::IsBeingSeenByPlayer(const APlayerController* InPlayerController) const
{
	if (!IsValid(InPlayerController))
	{
		return false;
	}

	FVector PlayerViewLocation;
	FRotator PlayerViewRotation;
	InPlayerController->GetPlayerViewPoint(PlayerViewLocation, PlayerViewRotation);

	const FVector ViewDirection = PlayerViewRotation.Vector();
	FVector DistanceToPawnVector = GetActorLocation() - PlayerViewLocation;
	DistanceToPawnVector.Normalize();
	const float DotProduct = FVector::DotProduct(ViewDirection, DistanceToPawnVector);

	if (DotProduct < 0.f)
	{
		return false;
	}

	const float FieldOfView = InPlayerController->PlayerCameraManager->GetFOVAngle();
	const float HalfFOV = FMath::DegreesToRadians(FieldOfView / 2.0f);
	if (FMath::Acos(DotProduct) >= HalfFOV)
	{
		return false;
	}

	if (!InPlayerController->LineOfSightTo(this, PlayerViewLocation, true))
	{
		return false;
	}

	return true;
}

void ALimenAICharacter::ResumeBrainLogic(const FString& Reason)
{
	LimenAIController->ResumeBrainLogic(Reason);
}

void ALimenAICharacter::PauseBrainLogic(const FString& Reason)
{
	LimenAIController->PauseBrainLogic(Reason);
}

void ALimenAICharacter::Interact()
{
	Super::Interact();

	InteractionComponent->Interact(GetController(), this);
}

ULimenInteractionComponent* ALimenAICharacter::GetInteractionComponent() const
{
	return InteractionComponent.Get();
}
