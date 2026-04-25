// Copyright Face Software. All Rights Reserved.


#include "PlayerController/LimenPlayerControllerBase.h"

#include "TimerManager.h"
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
#include "EngineUtils.h"


ALimenPlayerControllerBase* ALimenPlayerControllerBase::GetLimenPlayerControllerBase(UObject* Caller, int32 PlayerIndex)
{
	const UWorld* World = GEngine->GetWorldFromContextObject(Caller, EGetWorldErrorMode::ReturnNull);
	if (!World) { return nullptr; }

	int32 Index = 0;
	for (TActorIterator<ALimenPlayerControllerBase> It(World); It; ++It)
	{
		if (Index == PlayerIndex) return *It;
		Index++;
	}

	return nullptr;
}

ALimenPlayerControllerBase::ALimenPlayerControllerBase(const FObjectInitializer& InObjectInitializer) : Super(InObjectInitializer)
{
	PrimaryActorTick.bTickEvenWhenPaused = true;
	CurrentInputMode = ELimenInputMode::Undefined;
	bPawnBeginPlayBound = false;
	MouseYawMultiplier = 1.f;
	MousePitchMultiplier = 1.f;

	MouseInputSensitivityComponent = CreateDefaultSubobject<ULimenMouseSensitivityComponent>(TEXT("MouseInputSensitivityComponent"));
}

void ALimenPlayerControllerBase::BeginPlay()
{
	Super::BeginPlay();

	if (ULimenLevelTransitionSubsystem* LevelTransitionHandler = GetGameInstance()->GetSubsystem<ULimenLevelTransitionSubsystem>(); LevelTransitionHandler != nullptr)
	{
		LevelTransitionHandler->OnLoadingScreenVisibilityChanged.AddUObject(this, &ThisClass::LoadingScreenVisibilityChanged);
		LoadingScreenVisibilityChanged(LevelTransitionHandler->IsLoadingScreenActive());
	}
	
	MouseInputSensitivityComponent->OnSensitivityUpdated.AddUniqueDynamic(this, &ThisClass::SensitivityUpdated);
	SensitivityUpdated(MouseInputSensitivityComponent.Get());

	if (HasAuthority() && LimenBaseHUD.IsValid() && GetPawn() == nullptr)
	{
		LimenBaseHUD->UpdateWidgets(this, GetPawn());
		BindWidgetDelegates();
	}
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

	LIMEN_LOG(LogLimenCore, Log, this, TEXT("Game input set"))
}

void ALimenPlayerControllerBase::SetUIInput()
{	
	// Do not flush or this will behave the same as UI Only
	FInputModeGameAndUI InputMode;
	InputMode.SetHideCursorDuringCapture(false);
	SetInputMode(InputMode);
	SetShowMouseCursor(true);
	CurrentInputMode = ELimenInputMode::UI;
	
	LIMEN_LOG(LogLimenCore, Log, this, TEXT("UI input set"))
}

void ALimenPlayerControllerBase::SetUIOnlyInput(const bool bShowMouse /* = true */)
{
	FlushPressedKeys();
	SetInputMode(FInputModeUIOnly());
	SetShowMouseCursor(bShowMouse);
	CurrentInputMode = ELimenInputMode::UIOnly;

	LIMEN_LOG(LogLimenCore, Log, this, TEXT("UI Only input set"))
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
	check(NewPawn)
	if (NewPawn == GetPawn())
	{
		GetPawn()->OnPawnBeginPlay.RemoveAll(this);
	}
}

void ALimenPlayerControllerBase::UnbindPawnDelegates(APawn* InPawn)
{
	check(InPawn)
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

void ALimenPlayerControllerBase::BlendViewTargetSet(AActor* NewViewTarget)
{
}

void ALimenPlayerControllerBase::SetCurrentInputMode(const ELimenInputMode InInputMode)
{
	CurrentInputMode = InInputMode;
}

void ALimenPlayerControllerBase::SetPawn(APawn* InPawn)
{
	APawn* PrevPawn = GetPawn();

	Super::SetPawn(InPawn);

	if (PrevPawn) UnbindPawnDelegates(PrevPawn);

	if (!InPawn) return;

	BindPawnDelegates(InPawn);

	if (!GetWorld()->bIsTearingDown && LimenBaseHUD.IsValid())
	{
		UnbindWidgetDelegates();
		LimenBaseHUD->UpdateWidgets(this, InPawn);
		BindWidgetDelegates();
	}

	if (!InPawn->HasActorBegunPlay())
	{
		InPawn->OnPawnBeginPlay.AddUObject(this, &ThisClass::CurrentPawnReadyInternal);
	}
	else
	{
		CurrentPawnReadyInternal(InPawn);
	}
}

void ALimenPlayerControllerBase::ClientSetHUD_Implementation(TSubclassOf<AHUD> NewHUDClass)
{
	Super::ClientSetHUD_Implementation(NewHUDClass);

	LimenBaseHUD = Cast<ALimenBaseHUD>(GetHUD());
}

void ALimenPlayerControllerBase::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	if (!GetWorld()->bIsTearingDown && LimenBaseHUD.IsValid())
	{
		LimenBaseHUD->UpdateWidgets(this, GetPawn());
		BindWidgetDelegates();
	}
}

void ALimenPlayerControllerBase::SetViewTargetWithBlend(AActor* NewViewTarget, const float BlendTime,
	const EViewTargetBlendFunction BlendFunc, const float BlendExp, const bool bLockOutgoing)
{
	Super::SetViewTargetWithBlend(NewViewTarget, BlendTime, BlendFunc, BlendExp, bLockOutgoing);

	if (FMath::IsNearlyEqual(BlendTime, 0.f))
	{
		BlendViewTargetSet(NewViewTarget);
	}
	else
	{
		GetWorld()->GetTimerManager().SetTimer(ViewTargetTransitionTimerHandle, [this, NewViewTarget]
		{
			BlendViewTargetSet(NewViewTarget);
		}
		, BlendTime, false);
	}
}

void ALimenPlayerControllerBase::BindWidgetDelegates()
{
}

void ALimenPlayerControllerBase::UnbindWidgetDelegates()
{
}

void ALimenPlayerControllerBase::CurrentPawnReady()
{
	if (HasAuthority())
	{
		BindPawnDelegates(GetPawn());

		if (!GetWorld()->bIsTearingDown && LimenBaseHUD.IsValid())
		{
			LimenBaseHUD->UpdateWidgets(this, GetPawn());
			BindWidgetDelegates();
		}
	}
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

void ALimenPlayerControllerBase::CurrentPawnReadyInternal(APawn* InPawn)
{
	if (!IsValid(InPawn) || InPawn != GetPawn())
	{
		return;
	}

	CurrentPawnReady();
}
