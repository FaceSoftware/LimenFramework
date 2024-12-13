// Copyright Face Software. All Rights Reserved.


#include "Components/LimenCameraShakeComponent.h"


ULimenCameraShakeComponent::ULimenCameraShakeComponent()
{
	CameraShakePlaySpace = ECameraShakePlaySpace::CameraLocal;
}

void ULimenCameraShakeComponent::PlayStandCameraShake()
{
	if (!IsActive() || !StandCameraShake || ActiveCameraShakeClass == StandCameraShake)
	{
		return;
	}

	ActiveCameraShakeClass = StandCameraShake;
	PlayCameraShake();
}

void ULimenCameraShakeComponent::PlayWalkCameraShake()
{
	if (!IsActive() || !WalkCameraShake || ActiveCameraShakeClass == WalkCameraShake)
	{
		return;
	}
	
	ActiveCameraShakeClass = WalkCameraShake;
	PlayCameraShake();
}

void ULimenCameraShakeComponent::PlaySprintCameraShake()
{
	if (!IsActive() || !SprintCameraShake || ActiveCameraShakeClass == SprintCameraShake)
	{
		return;
	}

	ActiveCameraShakeClass = SprintCameraShake;
	PlayCameraShake();
}

void ULimenCameraShakeComponent::StopCameraShake()
{
	if (!ActiveCameraShake)
	{
		return;
	}
	
	ActiveCameraShake->StopShake(false);
	// Let garbage collection have these ones
	ActiveCameraShake = nullptr;
	ActiveCameraShakeClass = nullptr;
}

bool ULimenCameraShakeComponent::IsPlayingCameraShake() const
{
	return ActiveCameraShake != nullptr;
}

bool ULimenCameraShakeComponent::IsPlayingStandCameraShake() const
{
	return IsPlayingCameraShake() && ActiveCameraShake->GetClass() == StandCameraShake;
}

bool ULimenCameraShakeComponent::IsPlayingWalkCameraShake() const
{
	return IsPlayingCameraShake() && ActiveCameraShake->GetClass() == WalkCameraShake;
}

bool ULimenCameraShakeComponent::IsPlayingSprintCameraShake() const
{
	return IsPlayingCameraShake() && ActiveCameraShake->GetClass() == SprintCameraShake;
}

void ULimenCameraShakeComponent::PlayCameraShake()
{
	check(ActiveCameraShakeClass)
	StopCameraShake();
	ActiveCameraShake = GetWorld()->GetFirstPlayerController()->PlayerCameraManager->StartCameraShake(ActiveCameraShakeClass, 1.f, CameraShakePlaySpace);
}

void ULimenCameraShakeComponent::SetActive(bool bNewActive, bool bReset)
{
	Super::SetActive(bNewActive, bReset);

	if (bNewActive)
	{
		
	}
	else
	{
		StopCameraShake();
	}
}
