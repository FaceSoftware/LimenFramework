// Copyright Face Software. All Rights Reserved.


#include "Characters/LimenCharacterBase.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/LimenMovementComponent.h"
#include "Components/LimenNotificationComponent.h"
#include "CppClasses/LimenNotification.h"
#include "GameFramework/HUD.h"
#include "LimenAbilitySystem/Public/Components/LimenAbilityComponent.h"
#include "LogMacros/LimenLogMacros.h"
#include "PlayerControllers/LimenPlayerControllerBase.h"


ALimenCharacterBase::ALimenCharacterBase(const FObjectInitializer& InObjectInitializer) : Super(InObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	bUseControllerRotationYaw = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->PrimaryComponentTick.bCanEverTick = true;
	GetCharacterMovement()->PrimaryComponentTick.TickInterval = 0.0078125f;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->bCanWalkOffLedges = true;
	GetCharacterMovement()->bCanWalkOffLedgesWhenCrouching = true;
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	GetCharacterMovement()->NavAgentProps.bCanFly = false;
	GetCharacterMovement()->NavAgentProps.bCanJump = true;
	GetCharacterMovement()->NavAgentProps.bCanSwim = false;
	GetCharacterMovement()->NavAgentProps.bCanWalk = true;
	GetCharacterMovement()->bUseFlatBaseForFloorChecks = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->MaxWalkSpeed = 400.f;

	GetMesh()->SetOnlyOwnerSee(false);
	GetMesh()->SetOwnerNoSee(true);
	GetMesh()->SetCastShadow(true);
	GetMesh()->bEditableWhenInherited = true;
}

void ALimenCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	auto* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	check(EnhancedInput)

	EnhancedInput->BindAction(MoveInputAction.LoadSynchronous(), ETriggerEvent::Triggered, this, &ThisClass::MoveInput);
	EnhancedInput->BindAction(LookInputAction.LoadSynchronous(), ETriggerEvent::Triggered, this, &ThisClass::LookInput);
}

bool ALimenCharacterBase::QueueNotification(const FNotificationParams& InParams)
{
	if (PlayerController == nullptr)
	{
		LIMEN_LOG(LogLimen, Error, this, "Cannot queue notifications to AI controlled characters")
		return false;
	}

	auto* NotificationComponent = PlayerController->GetHUD()->GetComponentByClass<ULimenNotificationComponent>();
	if (!IsValid(NotificationComponent))
	{
		LIMEN_LOG(LogLimen, Error, this, "Could not find a notification component in the player's HUD class")
		return false;
	}

	NotificationComponent->QueueNotification(MakeShared<FLimenNotification>(InParams));
	return true;
}

ALimenPlayerControllerBase* ALimenCharacterBase::GetLimenBasePlayerController() const
{
	return LimenBasePlayerController.Get();
}

const FMouseParameters& ALimenCharacterBase::GetMouseParameters() const
{
	return MouseParameters;
}

void ALimenCharacterBase::SetMouseParameters(const FMouseParameters& InNewParams)
{
	MouseParameters = InNewParams;
}

APlayerController* ALimenCharacterBase::GetPlayerController() const
{
	return PlayerController.Get();
}

const TSoftObjectPtr<UInputAction>& ALimenCharacterBase::GetMoveInputAction() const
{
	return MoveInputAction;
}

const TSoftObjectPtr<UInputAction>& ALimenCharacterBase::GetLookInputAction() const
{
	return LookInputAction;
}

const TSoftObjectPtr<UInputAction>& ALimenCharacterBase::GetSprintInputAction() const
{
	return SprintInputAction;
}

void ALimenCharacterBase::MoveInput(const FInputActionInstance& Instance)
{
	if (const FVector2D Input = Instance.GetValue().Get<FVector2D>(); !Input.IsZero())
	{
		AddMovementInput(GetActorForwardVector(), Input.X);
		AddMovementInput(GetActorRightVector(), Input.Y);
	}
}

void ALimenCharacterBase::LookInput(const FInputActionInstance& Instance)
{
	const FVector2D Input = Instance.GetValue().Get<FVector2D>();
	if (!Input.IsZero())
	{
		AddControllerYawInput((MouseParameters.bInvertAxisX ? -Input.X : Input.X) * MouseParameters.SensitivityX);
		AddControllerPitchInput((MouseParameters.bInvertAxisY ? Input.Y : -Input.Y) * MouseParameters.SensitivityY);
	}
}

void ALimenCharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	PlayerController = Cast<APlayerController>(NewController);
	LimenBasePlayerController = Cast<ALimenPlayerControllerBase>(NewController);
}
