// Copyright Face Software. All Rights Reserved.


#include "Characters/LimenCharacterBase.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Abilities/LimenVariableMovementAbility.h"
#include "Components/LimenMovementComponent.h"
#include "Components/LimenNotificationComponent.h"
#include "CppClasses/LimenNotification.h"
#include "GameFramework/HUD.h"
#include "LimenAbilitySystem/Public/Components/LimenAbilityComponent.h"
#include "LogMacros/LimenLogMacros.h"
#include "PlayerControllers/LimenPlayerControllerBase.h"
#include "PlayerStates/LimenBasePlayerState.h"


ALimenCharacterBase::ALimenCharacterBase(const FObjectInitializer& InObjectInitializer) : Super(InObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

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

	AbilityComponent = CreateDefaultSubobject<ULimenAbilityComponent>(TEXT("AbilityComponent"));
}

void ALimenCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	VariableMovementAbility = GetAbilityComponent()->GetAbility<ULimenVariableMovementAbility>();
}

void ALimenCharacterBase::EnableInput(class APlayerController* InPlayerController)
{
	Super::EnableInput(InPlayerController);
	
	GetComponentByClass<UEnhancedInputComponent>()->Activate();
}

void ALimenCharacterBase::DisableInput(class APlayerController* InPlayerController)
{
	Super::DisableInput(InPlayerController);
	
	GetComponentByClass<UEnhancedInputComponent>()->Deactivate();
}

void ALimenCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	auto* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	check(EnhancedInput)
	
	EnhancedInput->BindAction(MoveInputAction.LoadSynchronous(), ETriggerEvent::Triggered, this, &ThisClass::MoveInput);
	EnhancedInput->BindAction(LookInputAction.LoadSynchronous(), ETriggerEvent::Triggered, this, &ThisClass::LookInput);
	EnhancedInput->BindAction(SprintInputAction.LoadSynchronous(), ETriggerEvent::Triggered, this, &ThisClass::SprintInput);
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

void ALimenCharacterBase::StartSprinting()
{
	if (VariableMovementAbility == nullptr)
	{
		return;
	}

	if (VariableMovementAbility->CanActivateAbility())
	{
		VariableMovementAbility->ActivateAbility(GetController(), this);
	}
}

void ALimenCharacterBase::StopSprinting()
{
	if (VariableMovementAbility == nullptr)
	{
		return;
	}
	
	VariableMovementAbility->CancelAbility(GetController(), this);
}

void ALimenCharacterBase::ToggleSprint()
{
	if (VariableMovementAbility == nullptr)
	{
		return;
	}
	
	if (VariableMovementAbility->IsActive())
	{
		StartSprinting();
	}
	else
	{
		StopSprinting();
	}
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

ULimenAbilityComponent* ALimenCharacterBase::GetAbilityComponent() const
{
	return AbilityComponent.Get();
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

FVector ALimenCharacterBase::GetLookTarget(const float MaxDistance) const
{
	FVector EyesLocation;
	FRotator EyesDirection;
	GetActorEyesViewPoint(EyesLocation, EyesDirection);

	FVector& Start = EyesLocation;
	FVector End = EyesLocation + (EyesDirection.Vector() * MaxDistance);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	FHitResult Hit;
	if (!GetWorld()->LineTraceSingleByChannel(Hit, EyesLocation,
											  EyesLocation + (EyesDirection.Vector() * MaxDistance),
											  ECollisionChannel::ECC_Visibility, Params))
	{
		return End;
	}

	return Hit.Location;
}

void ALimenCharacterBase::OnHealthAttributeEmpty(const float NewValue)
{
	GetCharacterMovement()->DisableMovement();
	AbilityComponent->DeactivateAllAbilities();
}

void ALimenCharacterBase::MoveInput(const FInputActionInstance& Instance)
{
	OnMovementInput.Broadcast();
	const auto Input = Instance.GetValue().Get<FVector2D>();
	if (!Input.IsZero())
	{
		AddMovementInput(GetActorForwardVector(), Input.X);
		AddMovementInput(GetActorRightVector(), Input.Y);
	}
}

void ALimenCharacterBase::LookInput(const FInputActionInstance& Instance)
{
	const auto Input = Instance.GetValue().Get<FVector2D>();
	AddControllerYawInput((MouseParameters.bInvertAxisX ? -Input.X : Input.X) * MouseParameters.SensitivityX);
	AddControllerPitchInput((MouseParameters.bInvertAxisY ? Input.Y : -Input.Y) * MouseParameters.SensitivityY);
}

void ALimenCharacterBase::SprintInput(const FInputActionInstance& Instance)
{
	OnSprintInput.Broadcast();
	if (Instance.GetValue().Get<bool>())
	{
		StartSprinting();
	}
	else
	{
		StopSprinting();
	}
}

void ALimenCharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	PlayerController = Cast<APlayerController>(NewController);
	LimenBasePlayerController = Cast<ALimenPlayerControllerBase>(NewController);
	LimenBasePlayerState = NewController->GetPlayerState<ALimenBasePlayerState>();

	if (!AbilityComponent->IsReadyForGameplay())
	{
		AbilityComponent->LoadAbilities(this);
		AbilityComponent->LoadAttributes(this);
	}
}

ALimenBasePlayerState* ALimenCharacterBase::GetLimenBasePlayerState() const
{
	return LimenBasePlayerState.Get();
}
