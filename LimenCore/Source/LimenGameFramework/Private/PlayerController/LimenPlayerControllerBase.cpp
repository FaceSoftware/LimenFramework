// Copyright Face Software. All Rights Reserved.


#include "PlayerController/LimenPlayerControllerBase.h"

#include "Components/LimenMouseSensitivityComponent.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "GameFramework/HUD.h"
#include "GameFramework/Pawn.h"
#include "HUD/LimenBaseHUD.h"
#include "LogMacros/LimenLogMacros.h"
#include "Subsystems/LimenLevelTransitionSubsystem.h"
#include "UMG/LimenLoadingScreenWidget.h"


ALimenPlayerControllerBase* ALimenPlayerControllerBase::GetLimenPlayerControllerBase(UObject* Caller, int32 PlayerIndex)
{
	const UWorld* World = GEngine->GetWorldFromContextObject(Caller, EGetWorldErrorMode::ReturnNull);
	if (World == nullptr)
	{
		return nullptr;
	}

	FConstPlayerControllerIterator It = World->GetPlayerControllerIterator();
	for (int i = 0; i < PlayerIndex; ++i) ++It;
	if (It.GetIndex() != PlayerIndex)
	{
		return nullptr;
	}

	return Cast<ALimenPlayerControllerBase>(*It);
}

ALimenPlayerControllerBase::ALimenPlayerControllerBase(const FObjectInitializer& InObjectInitializer) : Super(InObjectInitializer)
{
	PrimaryActorTick.bTickEvenWhenPaused = true;
	CurrentInputMode = ELimenInputMode::Undefined;
	MouseYawMultiplier = 1.f;
	MousePitchMultiplier = 1.f;

	MouseInputSensitivityComponent = CreateDefaultSubobject<ULimenMouseSensitivityComponent>(TEXT("MouseInputSensitivityComponent"));
}

void ALimenPlayerControllerBase::BeginPlay()
{
	Super::BeginPlay();
	
	MouseInputSensitivityComponent->OnSensitivityUpdated.AddUniqueDynamic(this, &ThisClass::SensitivityUpdated);
	SensitivityUpdated(MouseInputSensitivityComponent.Get());

	if (CreateHudReference() && GetPawn() == nullptr)
	{
		LimenBaseHUD->UpdateWidgets(this, GetPawn());
		BindWidgetDelegates();
	}
}

void ALimenPlayerControllerBase::SetupInputComponent()
{
	Super::SetupInputComponent();

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
	CurrentInputMode = ELimenInputMode::Game;

	LIMEN_LOG(LogLimenCore, Log, this, "Game input set");
}

void ALimenPlayerControllerBase::SetUIInput()
{	
	// Do not flush or this will behave the same as UI Only
	FInputModeGameAndUI InputMode;
	InputMode.SetHideCursorDuringCapture(false);
	SetInputMode(InputMode);
	SetShowMouseCursor(true);
	CurrentInputMode = ELimenInputMode::UI;
	
	LIMEN_LOG(LogLimenCore, Log, this, "UI input set");
}

void ALimenPlayerControllerBase::SetUIOnlyInput(const bool bShowMouse /* = true */)
{
	FlushPressedKeys();
	SetInputMode(FInputModeUIOnly());
	SetShowMouseCursor(bShowMouse);
	CurrentInputMode = ELimenInputMode::UIOnly;

	LIMEN_LOG(LogLimenCore, Log, this, "UI Only input set")
}

void ALimenPlayerControllerBase::SetInputMode(const FInputModeDataBase& InData)
{
	Super::SetInputMode(InData);
}

void ALimenPlayerControllerBase::SetInputMode(const ELimenInputMode InInputMode)
{
	CurrentInputMode = InInputMode;
	switch (CurrentInputMode)
	{		
	case ELimenInputMode::Game:
		SetGameInput();
		break;
		
	case ELimenInputMode::UI:
		SetUIInput();
		break;
		
	case ELimenInputMode::UIOnly:
		SetUIOnlyInput();
		break;

	default:
		checkNoEntry()
		break;
	}
}

ELimenInputMode ALimenPlayerControllerBase::GetInputMode() const
{
	return CurrentInputMode;
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

void ALimenPlayerControllerBase::AddYawInput(const float Val)
{
	Super::AddYawInput(Val * MouseYawMultiplier);
}

void ALimenPlayerControllerBase::AddPitchInput(const float Val)
{
	Super::AddPitchInput(Val * MousePitchMultiplier);
}

void ALimenPlayerControllerBase::BindPawnDelegates(APawn* NewPawn)
{
	if (NewPawn == GetPawn())
	{
		GetPawn()->OnPawnBeginPlay.RemoveAll(this);
	}
}

void ALimenPlayerControllerBase::UnbindPawnDelegates(APawn* InPawn)
{
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

void ALimenPlayerControllerBase::BindWidgetDelegates()
{
}

void ALimenPlayerControllerBase::UnbindWidgetDelegates()
{
}

void ALimenPlayerControllerBase::SensitivityUpdated(ULimenMouseSensitivityComponent* Component)
{
	{
		const float MouseMultiplier = MouseInputSensitivityComponent->GetMouseX();
		const bool bInvert = MouseInputSensitivityComponent->GetInvertMouseX();
		MouseYawMultiplier = MouseMultiplier * (bInvert ? -1.0f : 1.0f);
	}

	{
		const float MouseMultiplier = MouseInputSensitivityComponent->GetMouseY();
		const bool bInvert = MouseInputSensitivityComponent->GetInvertMouseY();
		MousePitchMultiplier = MouseMultiplier * (bInvert ? -1.0f : 1.0f);
	}
}
