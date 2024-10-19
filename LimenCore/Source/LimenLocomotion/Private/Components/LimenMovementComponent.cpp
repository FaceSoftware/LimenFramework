// Copyright Face Software. All Rights Reserved.


#include "Components/LimenMovementComponent.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "HAL/RunnableThread.h"
#include "LimenCore/Public/LogMacros/LimenLogMacros.h"


UDEPRECATED_LimenMovementComponent::UDEPRECATED_LimenMovementComponent()
{

	
	bWantsToSprint = false;
	MovementMode = MOVE_Walking;
	CustomMovementMode = static_cast<uint8>(ECustomMovementMode::None);
}

void UDEPRECATED_LimenMovementComponent::OnComponentCreated()
{
	Super::OnComponentCreated();
}

void UDEPRECATED_LimenMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateMovementMode();
}

void UDEPRECATED_LimenMovementComponent::ToggleSprint()
{
	if (CustomMovementMode == CustomMovementModeToByte(ECustomMovementMode::Sprinting))
	{
		StopSprinting();
	}
	else
	{
		StartSprinting();
	}
}

void UDEPRECATED_LimenMovementComponent::StartSprinting()
{
	if (!WantsToSprint())
	{
		bWantsToSprint = true;
	}
}

void UDEPRECATED_LimenMovementComponent::StopSprinting()
{
	if (WantsToSprint())
	{
		bWantsToSprint = false;
	}
}

bool UDEPRECATED_LimenMovementComponent::IsSprinting() const
{
	return CustomMovementModeToByte(ECustomMovementMode::Sprinting) == CustomMovementMode;
}

bool UDEPRECATED_LimenMovementComponent::WantsToSprint() const
{
	return bWantsToSprint;
}

bool UDEPRECATED_LimenMovementComponent::IsStill() const
{
	return Velocity.Length() <= UE_KINDA_SMALL_NUMBER;
}

ECustomMovementMode UDEPRECATED_LimenMovementComponent::GetCustomMovementMode() const
{
	return static_cast<ECustomMovementMode>(CustomMovementMode);
}

void UDEPRECATED_LimenMovementComponent::SetMovementMode(EMovementMode NewMovementMode, uint8 NewCustomMode)
{
	Super::SetMovementMode(NewMovementMode, NewCustomMode);
	
	switch (static_cast<ECustomMovementMode>(NewCustomMode))
	{
	case ECustomMovementMode::None:
		MaxCustomMovementSpeed = 0;
		break;
		
	case ECustomMovementMode::CrouchWalking:
		MaxCustomMovementSpeed = MaxCrouchWalkingSpeed;
		break;
		
	case ECustomMovementMode::Sprinting:
		MaxCustomMovementSpeed = MaxSprintSpeed;
		break;
		
	case ECustomMovementMode::CrouchSprinting:
		MaxCustomMovementSpeed = MaxCrouchSprintSpeed;
		break;
	}
}

void UDEPRECATED_LimenMovementComponent::UpdateMovementMode()
{
	if (!CanSprint() && IsSprinting() || !bWantsToSprint && IsSprinting())
	{
		if (IsCrouching())
		{
			OnMovementStatusUpdated.Broadcast();
			LIMEN_LOG(LogLimenCore, Log, this, "Player is crouching and walking")
			SetMovementMode(MOVE_Custom, CustomMovementModeToByte(ECustomMovementMode::CrouchWalking));
		}
		else
		{
			OnMovementStatusUpdated.Broadcast();
			LIMEN_LOG(LogLimenCore, Log, this, "Player is walking")
			SetMovementMode(MOVE_Walking, CustomMovementModeToByte(ECustomMovementMode::None));
		}
	}
	else if (CanSprint() && !IsSprinting() && bWantsToSprint)
	{
		if (IsCrouching())
		{
			OnMovementStatusUpdated.Broadcast();
			LIMEN_LOG(LogLimenCore, Log, this, "Player is crouch sprinting")
			SetMovementMode(MOVE_Custom, CustomMovementModeToByte(ECustomMovementMode::CrouchSprinting));
		}
		else
		{
			OnMovementStatusUpdated.Broadcast();
			LIMEN_LOG(LogLimenCore, Log, this, "Player is sprinting")
			SetMovementMode(MOVE_Custom, CustomMovementModeToByte(ECustomMovementMode::Sprinting));
		}
	}
}

bool UDEPRECATED_LimenMovementComponent::CanSprint()
{
	return !IsFalling() && Velocity.Length() > UE_KINDA_SMALL_NUMBER;
}
