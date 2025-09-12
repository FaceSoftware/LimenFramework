// Copyright Face Software. All Rights Reserved.


#include "Pawn/LimenCharacterBase.h"

#include "AIController.h"
#include "EnhancedInputComponent.h"
#include "Components/LimenMovementComponent.h"
#include "Components/LimenNotificationComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "CppClasses/LimenNotification.h"
#include "GameFramework/HUD.h"
#include "Components/LimenAbilityComponent.h"
#include "LogMacros/LimenLogMacros.h"
#include "PlayerController/LimenPlayerControllerBase.h"
#include "PlayerState/LimenPlayerStateBase.h"
#include "Attributes/LimenHealthAttribute.h"
#include "Components/CapsuleComponent.h"


ALimenCharacterBase::ALimenCharacterBase(const FObjectInitializer& InObjectInitializer)
	: Super(InObjectInitializer.
		SetDefaultSubobjectClass<ULimenMovementComponent>(CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationYaw = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	GetCapsuleComponent()->SetCanEverAffectNavigation(true);
	
	GetCharacterMovement()->PrimaryComponentTick.bCanEverTick = true;
	GetCharacterMovement()->PrimaryComponentTick.TickInterval = 0.f;
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

	if (GetMesh())
	{
		GetMesh()->SetOnlyOwnerSee(false);
		GetMesh()->SetOwnerNoSee(false);
		GetMesh()->SetCastShadow(true);
		GetMesh()->bEditableWhenInherited = true;
	}

	AbilityComponent = CreateDefaultSubobject<ULimenAbilityComponent>(TEXT("AbilityComponent"));
}

void ALimenCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	SetupAbilityComponentInternal(AbilityComponent.Get());
}

void ALimenCharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	PlayerController = Cast<APlayerController>(NewController);
	LimenBasePlayerController = Cast<ALimenPlayerControllerBase>(NewController);
	LimenBasePlayerState = NewController->GetPlayerState<ALimenPlayerStateBase>();
}

void ALimenCharacterBase::EnableInput(APlayerController* InPlayerController)
{
	Super::EnableInput(InPlayerController);

	
	if (InPlayerController == PlayerController)
	{
		GetComponentByClass<UEnhancedInputComponent>()->Activate();
	}
	
}

void ALimenCharacterBase::DisableInput(APlayerController* InPlayerController)
{
	Super::DisableInput(InPlayerController);

	if (InPlayerController == PlayerController)
	{
		GetComponentByClass<UEnhancedInputComponent>()->Deactivate();
	}
}

bool ALimenCharacterBase::CanJumpInternal_Implementation() const
{
	// return Super::CanJumpInternal_Implementation();
	return JumpIsAllowedInternal();
}

void ALimenCharacterBase::SetCinematicMode(bool bInCinematicMode, bool bHidePlayer, bool bAffectsHUD,
										   bool bAffectsMovement, bool bAffectsTurning)
{
	check(HasAuthority())

	Multicast_SetCinematicMode(bInCinematicMode, bAffectsHUD, bAffectsMovement, bAffectsTurning);
}

bool ALimenCharacterBase::QueueNotification(const FNotificationParams& InParams)
{
	if (Controller->IsA<AAIController>())
	{
		LIMEN_LOG(LogLimen, Error, this, TEXT("Cannot queue notifications to AI controlled characters"))
		return false;
	}

	ULimenNotificationComponent* NotificationComponent = PlayerController->GetHUD()->GetComponentByClass<ULimenNotificationComponent>();
	if (NotificationComponent == nullptr)
	{
		LIMEN_LOG(LogLimen, Error, this, TEXT("Could not find a notification component in the player's HUD class"))
		return false;
	}

	NotificationComponent->QueueNotification(MakeShared<FLimenNotification>(InParams));
	return true;
}

ALimenPlayerControllerBase* ALimenCharacterBase::GetLimenBasePlayerController() const
{
	return LimenBasePlayerController.Get();
}

ULimenAbilityComponent* ALimenCharacterBase::GetAbilityComponent() const
{
	return AbilityComponent.Get();
}

APlayerController* ALimenCharacterBase::GetPlayerController() const
{
	return PlayerController.Get();
}

FVector ALimenCharacterBase::GetLookTarget(const float MaxDistance) const
{
	FVector EyesLocation;
	FRotator EyesDirection;
	GetActorEyesViewPoint(EyesLocation, EyesDirection);

	const FVector& Start = EyesLocation;
	const FVector End = EyesLocation + (EyesDirection.Vector() * MaxDistance);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	FHitResult Hit;
	if (!GetWorld()->LineTraceSingleByChannel(Hit, Start,
											  EyesLocation + (EyesDirection.Vector() * MaxDistance),
											  ECollisionChannel::ECC_Visibility, Params))
	{
		return End;
	}

	return Hit.Location;
}

ALimenPlayerStateBase* ALimenCharacterBase::GetLimenBasePlayerState() const
{
	return LimenBasePlayerState.Get();
}

void ALimenCharacterBase::SetupAbilityComponent(ULimenAbilityComponent* InAbilityComponent)
{
}

void ALimenCharacterBase::Multicast_SetCinematicMode_Implementation(bool bInCinematicMode, bool bAffectsHUD,
	bool bAffectsMovement, bool bAffectsTurning)
{
}

void ALimenCharacterBase::SetupAbilityComponentInternal(ULimenAbilityComponent* InAbilityComponent)
{
	InAbilityComponent->OnAbilityComponentReady.AddUniqueDynamic(this, &ThisClass::SetupAbilityComponent);
	if (HasAuthority()) InAbilityComponent->Activate();
}
