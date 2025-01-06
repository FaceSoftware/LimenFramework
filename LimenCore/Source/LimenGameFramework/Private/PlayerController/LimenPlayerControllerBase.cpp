// Copyright Face Software. All Rights Reserved.


#include "PlayerController/LimenPlayerControllerBase.h"

#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Engine/GameInstance.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "GameFramework/HUD.h"
#include "GameFramework/Pawn.h"
#include "HUD/LimenBaseHUD.h"
#include "LogMacros/LimenLogMacros.h"
#include "Subsystems/LimenLevelTransitionSubsystem.h"


ALimenPlayerControllerBase::ALimenPlayerControllerBase(const FObjectInitializer& InObjectInitializer) : Super(InObjectInitializer)
{
	PrimaryActorTick.bTickEvenWhenPaused = true;
}

void ALimenPlayerControllerBase::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	const ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(Player);
	UEnhancedInputLocalPlayerSubsystem* InputSystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();

	if (!PlayerMappingContext.IsNull())
	{
		InputSystem->AddMappingContext(PlayerMappingContext.LoadSynchronous(), 1);
	}

	if (!CharacterMappingContext.IsNull())
	{
		InputSystem->AddMappingContext(CharacterMappingContext.LoadSynchronous(), 0);
	}

	ULimenLevelTransitionSubsystem* LevelTransitionHandler = GetGameInstance()->GetSubsystem<ULimenLevelTransitionSubsystem>();
	LevelTransitionHandler->OnLoadingScreenVisibilityChanged.AddUniqueDynamic(this, &ThisClass::LoadingScreenVisibilityChanged);
	LoadingScreenVisibilityChanged(LevelTransitionHandler->IsLoadingScreenActive());
}

void ALimenPlayerControllerBase::OnPossess(APawn* InPawn)
{	
	Super::OnPossess(InPawn);

	if (InPawn != nullptr)
	{
		if (InPawn->HasActorBegunPlay())
		{
			BindPawnDelegates(InPawn);
		}
		else
		{
			InPawn->OnPawnBeginPlay.AddUObject(this, &ThisClass::BindPawnDelegates);
		}
	}

	if (CreateHudReference())
	{
		LimenBaseHUD->UpdateWidgets(this, GetPawn());
		BindWidgetDelegates();
	}
}

void ALimenPlayerControllerBase::OnUnPossess()
{
	UnbindPawnDelegates(GetPawn());
	if (CreateHudReference())
	{
		UnbindWidgetDelegates();
	}
	
	Super::OnUnPossess();
}

void ALimenPlayerControllerBase::RequestPause(const EPauseReason Reason)
{
	if (GetWorld()->IsPaused())
	{
		return;
	}
	
	OnPauseRequested.Broadcast(this, Reason);
}

void ALimenPlayerControllerBase::RequestUnPause()
{
	if (!GetWorld()->IsPaused())
	{
		return;
	}
	
	OnUnPauseRequested.Broadcast(this);
}

void ALimenPlayerControllerBase::ToggleRequestPause(const EPauseReason Reason)
{
	if (GetWorld()->IsPaused())
	{
		RequestUnPause();
	}
	else
	{
		RequestPause(Reason);
	}
}

void ALimenPlayerControllerBase::SetGameInput()
{
	FlushPressedKeys();
	SetInputMode(FInputModeGameOnly());
	SetShowMouseCursor(false);

	LIMEN_LOG(LogLimenCore, Log, this, "Game input set");
}

void ALimenPlayerControllerBase::SetUIInput()
{	
	// Do not flush or this will behave the same as UI Only
	FInputModeGameAndUI InputMode;
	InputMode.SetHideCursorDuringCapture(false);
	SetInputMode(InputMode);
	SetShowMouseCursor(true);
	
	LIMEN_LOG(LogLimenCore, Log, this, "UI input set");
}

void ALimenPlayerControllerBase::SetUIOnlyInput(const bool bShowMouse /* = true */)
{
	FlushPressedKeys();
	SetInputMode(FInputModeUIOnly());
	SetShowMouseCursor(bShowMouse);

	LIMEN_LOG(LogLimenCore, Log, this, "UI Only input set")
}

bool ALimenPlayerControllerBase::CanSeeLocation(const FVector& InLocation) const
{
	FVector PlayerViewLocation;
	FRotator PlayerViewRotation;
	GetPlayerViewPoint(PlayerViewLocation, PlayerViewRotation);

	const FVector ViewDirection = PlayerViewRotation.Vector();
	FVector DistanceToLocationVector = InLocation - PlayerViewLocation;
	DistanceToLocationVector.Normalize();
	const float DotProduct = FVector::DotProduct(ViewDirection, DistanceToLocationVector);

	if (DotProduct < 0.f)
	{
		return false;
	}

	const float FieldOfView = PlayerCameraManager->GetFOVAngle();
	const float HalfFOV = FMath::DegreesToRadians(FieldOfView / 2.0f);
	if (FMath::Acos(DotProduct) >= HalfFOV)
	{
		return false;
	}
	
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(GetPawn());
	if (GetWorld()->LineTraceTestByChannel(PlayerViewLocation, InLocation, ECC_Visibility, QueryParams))
	{
		return false;
	}

	return true;
}

bool ALimenPlayerControllerBase::CanSeeActor(const AActor* OtherActor) const
{
	FVector PlayerViewLocation;
	FRotator PlayerViewRotation;
	GetPlayerViewPoint(PlayerViewLocation, PlayerViewRotation);

	const FVector ViewDirection = PlayerViewRotation.Vector();
	FVector DistanceToLocationVector = OtherActor->GetActorLocation() - PlayerViewLocation;
	DistanceToLocationVector.Normalize();
	const float DotProduct = FVector::DotProduct(ViewDirection, DistanceToLocationVector);

	if (DotProduct < 0.f)
	{
		return false;
	}

	const float FieldOfView = PlayerCameraManager->GetFOVAngle();
	const float HalfFOV = FMath::DegreesToRadians(FieldOfView / 2.0f);
	if (FMath::Acos(DotProduct) >= HalfFOV)
	{
		return false;
	}
	
	if (!LineOfSightTo(OtherActor))
	{
		return false;
	}

	return true;
}

void ALimenPlayerControllerBase::BindPawnDelegates(APawn* NewPawn)
{
	if (NewPawn == GetPawn())
	{
		GetPawn()->OnPawnBeginPlay.RemoveAll(this);
	}
}

void ALimenPlayerControllerBase::QueueNotification(const FNotificationParams& InParams)
{
	auto* HUDInstance = Cast<ALimenBaseHUD>(GetHUD());
	if (!IsValid(HUDInstance))
	{
		return;
	}

	HUDInstance->QueueNotification(InParams);
}

void ALimenPlayerControllerBase::LoadingScreenVisibilityChanged(const bool bIsVisible)
{
	if (bIsVisible)
	{
		DisableInput(this);
	}
	else // Is hidden
	{
		EnableInput(this);
	}
}

bool ALimenPlayerControllerBase::CreateHudReference()
{
	if (LimenBaseHUD.IsValid())
	{
		return true;
	}

	LimenBaseHUD = Cast<ALimenBaseHUD>(GetHUD());
	return LimenBaseHUD.IsValid();
}
