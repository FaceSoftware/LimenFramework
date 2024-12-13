// Copyright Face Software. All Rights Reserved.


#include "Actors/LimenCinematicActor.h"

#include "TimerManager.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PlayerController.h"
#include "Widgets/LimenBaseHudWidget.h"


const FName ALimenCinematicActor::MainCameraComponentName = TEXT("MainCamera");

ALimenCinematicActor::ALimenCinematicActor(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
	
	ViewTargetInTransitionParams.BlendExp = 1.f;
	ViewTargetInTransitionParams.BlendFunction = VTBlend_EaseIn;
	ViewTargetInTransitionParams.BlendTime = .0f;
	ViewTargetInTransitionParams.bLockOutgoing = false;
	
	ViewTargetOutTransitionParams.BlendExp = 1.f;
	ViewTargetOutTransitionParams.BlendFunction = VTBlend_EaseIn;
	ViewTargetOutTransitionParams.BlendTime = .0f;
	ViewTargetOutTransitionParams.bLockOutgoing = false;

	bIsCinematicPlaying = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	
	CameraComponent = CreateOptionalDefaultSubobject<UCameraComponent>(MainCameraComponentName);
	if (CameraComponent != nullptr)
	{
		CameraComponent->SetupAttachment(GetRootComponent());
	}
}

void ALimenCinematicActor::BeginPlay()
{
	Super::BeginPlay();

	if (bOverrideCamerasPostProcessSettings)
	{
		GetComponents<UCameraComponent>(CinematicCameras, false);
		for (UCameraComponent*& Camera : CinematicCameras)
		{
			Camera->PostProcessSettings = CinematicCamerasPostProcessSettings;
			Camera->PostProcessBlendWeight = 1.0f;
		}
	}
}

void ALimenCinematicActor::StartCinematic(APlayerController* InPlayerController)
{
	check(InPlayerController != nullptr);
	
	CinematicContextPlayerController = InPlayerController;
	OriginalViewTarget = OriginalViewTarget == nullptr ? CinematicContextPlayerController->GetViewTarget()
													   : OriginalViewTarget;

	CinematicContextPlayerController->SetViewTarget(this, ViewTargetInTransitionParams);

	DisableMainPostProcessSettings();
	
	bIsCinematicPlaying = true;
	
	if (FMath::IsNearlyEqual(ViewTargetInTransitionParams.BlendTime, 0.f))
	{
		OnViewTargetSetToSelf_Internal();
	}
	else
	{
		GetWorld()->GetTimerManager().SetTimer(ViewTargetTransitionTimerHandle, this,
										   &ThisClass::OnViewTargetSetToSelf_Internal,
										   ViewTargetInTransitionParams.BlendTime,
										   false);
	}

}

void ALimenCinematicActor::StopCinematic()
{
	CinematicContextPlayerController->SetViewTarget(OriginalViewTarget.Get(), ViewTargetOutTransitionParams);


	if (FMath::IsNearlyEqual(ViewTargetOutTransitionParams.BlendTime, 0.f))
	{
		OnViewTargetRestored_Internal();
	}
	else
	{
		GetWorld()->GetTimerManager().SetTimer(ViewTargetTransitionTimerHandle, this,
											   &ThisClass::OnViewTargetRestored_Internal,
											   ViewTargetOutTransitionParams.BlendTime,
											   false);
	}
}

void ALimenCinematicActor::CancelCinematic()
{
	EnableMainPostProcessSettings();
	
	OriginalViewTarget.Reset();
	CinematicContextPlayerController.Reset();

	bIsCinematicPlaying = false;
}

bool ALimenCinematicActor::IsCinematicPlaying() const
{
	return bIsCinematicPlaying;
}

AActor* ALimenCinematicActor::GetOriginalViewTarget() const
{
	return OriginalViewTarget.Get();
}

const FPostProcessSettings& ALimenCinematicActor::GetPostProcessSettings() const
{
	return CinematicCamerasPostProcessSettings;
}

void ALimenCinematicActor::SetPostProcessSettings(const FPostProcessSettings& NewSettings)
{
	CinematicCamerasPostProcessSettings = NewSettings;
}

void ALimenCinematicActor::OnViewTargetSetToSelf()
{ 
}

void ALimenCinematicActor::OnViewTargetRestored()
{
	OriginalViewTarget.Reset();
	CinematicContextPlayerController.Reset();
}

void ALimenCinematicActor::SetOriginalViewTarget(AActor* NewViewTarget)
{
	OriginalViewTarget = NewViewTarget;
}

void ALimenCinematicActor::EnableMainPostProcessSettings()
{
}

void ALimenCinematicActor::DisableMainPostProcessSettings()
{
}

void ALimenCinematicActor::OnViewTargetSetToSelf_Internal()
{
	GetWorld()->GetTimerManager().ClearTimer(ViewTargetTransitionTimerHandle);
	OnViewTargetSetToSelf();
	
	CinematicStart();
}

void ALimenCinematicActor::OnViewTargetRestored_Internal()
{
	GetWorld()->GetTimerManager().ClearTimer(ViewTargetTransitionTimerHandle);
	EnableMainPostProcessSettings();
	
	bIsCinematicPlaying = false;
	
	OnViewTargetRestored();
}
