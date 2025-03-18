// Copyright Face Software. All Rights Reserved.


#include "Pawn/LimenCharacterBase.h"

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

void ALimenCharacterBase::OnHealthAttributeEmpty(const float NewValue)
{
	GetCharacterMovement()->DisableMovement();
	AbilityComponent->DeactivateAllAbilities();
}

void ALimenCharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	PlayerController = Cast<APlayerController>(NewController);
	LimenBasePlayerController = Cast<ALimenPlayerControllerBase>(NewController);
	LimenBasePlayerState = NewController->GetPlayerState<ALimenPlayerStateBase>();

	if (!AbilityComponent->IsReadyForGameplay())
	{
		AbilityComponent->LoadAbilities(this);
		AbilityComponent->LoadAttributes(this);
	}
}

ALimenPlayerStateBase* ALimenCharacterBase::GetLimenBasePlayerState() const
{
	return LimenBasePlayerState.Get();
}
