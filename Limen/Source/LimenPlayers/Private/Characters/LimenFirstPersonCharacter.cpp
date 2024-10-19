// Copyright Face Software. All Rights Reserved.


#include "Characters/LimenFirstPersonCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/LimenCameraTiltComponent.h"
#include "Components/LimenPhysicalItemHoldComponent.h"
#include "Components/LimenVelocityBasedCameraShakeComponent.h"
#include "Components/Interaction/LimenLineTraceInteractionComponent.h"
#include "GameFramework/SpringArmComponent.h"


const FName ALimenFirstPersonCharacter::InteractionComponentName = TEXT("InteractionComponent");
const FName ALimenFirstPersonCharacter::CameraShakeComponentName = TEXT("CameraShakeComopnent");

ALimenFirstPersonCharacter::ALimenFirstPersonCharacter(const FObjectInitializer& InObjectInitializer) : Super(InObjectInitializer)
{
	CharacterInteraction = CreateDefaultSubobject<ULimenLineTraceInteractionComponent>(InteractionComponentName);
	CharacterInteraction->SetAutoActivate(true);

	CameraShakeComponent = CreateDefaultSubobject<ULimenVelocityBasedCameraShakeComponent>(CameraShakeComponentName);
	
	GetMesh()->SetOnlyOwnerSee(false);
	GetMesh()->SetOwnerNoSee(true);
	
	PlayerCamera->bUsePawnControlRotation = true;
	PlayerCamera->bEditableWhenInherited = true;
}

void ALimenFirstPersonCharacter::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

void ALimenFirstPersonCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	SetCameraHeightToEyeHeight();
	CharacterInteraction->OnInteract.AddUniqueDynamic(this, &ThisClass::OnInteract);
}

void ALimenFirstPersonCharacter::Interact()
{
	CharacterInteraction->Interact(GetController(), this);
}

void ALimenFirstPersonCharacter::StopInteraction()
{
	CharacterInteraction->StopInteraction(GetController(), this);
}

ULimenInteractionComponent* ALimenFirstPersonCharacter::GetInteractionComponent() const
{
	return CharacterInteraction.Get();
}

void ALimenFirstPersonCharacter::OnStartCrouch(const float HalfHeightAdjust, const float ScaledHalfHeightAdjust)
{
	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	SetCrouchedCameraPosition();
}

void ALimenFirstPersonCharacter::OnEndCrouch(const float HalfHeightAdjust, const float ScaledHalfHeightAdjust)
{
	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	SetStandingCameraPosition();
}

void ALimenFirstPersonCharacter::LookInput(const FInputActionInstance& Instance)
{
	Super::LookInput(Instance);

	const float YawInput = GetPlayerController()->RotationInput.Yaw;
	PlayerCamera->NotifyYawInput(YawInput);
}

void ALimenFirstPersonCharacter::SetCameraHeightToEyeHeight() const
{
	if (bIsCrouched)
	{
		SetCrouchedCameraPosition();
	}
	else
	{
		SetStandingCameraPosition();
	}
}

void ALimenFirstPersonCharacter::SetCrouchedCameraPosition() const
{
	FVector NewLocation = GetSpringArmComponent()->GetRelativeLocation();
	NewLocation.Z = CrouchedEyeHeight;
	GetSpringArmComponent()->SetRelativeLocation(NewLocation);
	PlayerCamera->SetRelativeLocation(FVector::ZeroVector);
}

void ALimenFirstPersonCharacter::SetStandingCameraPosition() const
{
	FVector NewLocation = GetSpringArmComponent()->GetRelativeLocation();
	NewLocation.Z = BaseEyeHeight;
	GetSpringArmComponent()->SetRelativeLocation(NewLocation);
	PlayerCamera->SetRelativeLocation(FVector::ZeroVector);
}
