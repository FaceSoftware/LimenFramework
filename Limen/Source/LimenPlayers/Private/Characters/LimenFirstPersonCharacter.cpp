// Copyright Face Software. All Rights Reserved.


#include "Characters/LimenFirstPersonCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/LimenCameraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/Interaction/LimenLineTraceInteractionComponent.h"


const FName ALimenFirstPersonCharacter::InteractionComponentName = TEXT("InteractionComponent");

ALimenFirstPersonCharacter::ALimenFirstPersonCharacter(const FObjectInitializer& InObjectInitializer) : Super(InObjectInitializer)
{
	CharacterInteraction = CreateDefaultSubobject<ULimenLineTraceInteractionComponent>(InteractionComponentName);
	CharacterInteraction->SetAutoActivate(true);
	
	GetMesh()->SetOnlyOwnerSee(false);
	GetMesh()->SetOwnerNoSee(true);
	
	PlayerCamera->bUsePawnControlRotation = true;
	PlayerCamera->bEditableWhenInherited = true;
}

void ALimenFirstPersonCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	CharacterInteraction->OnInteract.AddUniqueDynamic(this, &ThisClass::OnInteract);
}

void ALimenFirstPersonCharacter::Interact()
{
	// CharacterInteraction->Interact(GetController(), this);
}

void ALimenFirstPersonCharacter::StopInteraction()
{
	// CharacterInteraction->StopInteraction(GetController(), this);
}

ULimenInteractionComponent* ALimenFirstPersonCharacter::GetInteractionComponent() const
{
	return CharacterInteraction.Get();
}

void ALimenFirstPersonCharacter::LookInput(const FInputActionInstance& Instance)
{
	Super::LookInput(Instance);

	const float YawInput = GetPlayerController()->RotationInput.Yaw;
	PlayerCamera->NotifyYawInput(YawInput);
}
