// Copyright Face Software. All Rights Reserved.


#include "Components/LimenDynamicDepthOfFieldComponent.h"

#include "EngineUtils.h"
#include "BlueprintAsyncActions/LimenRecurrentAction.h"
#include "Engine/PostProcessVolume.h"


ULimenDynamicDepthOfFieldComponent::ULimenDynamicDepthOfFieldComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 1 / 32;
	GlobalPostProcessTag = TEXT("GlobalPostProcess");

	bAutoActivate = true;

	LineTraceChannel = ECC_Camera;
}

void ULimenDynamicDepthOfFieldComponent::BeginPlay()
{
	Super::BeginPlay();

	Deactivate();

	OwnerPawn = Cast<APawn>(GetOwner());
	check(OwnerPawn != nullptr);
	AController* Controller = OwnerPawn->GetController();
	if (Controller != nullptr)
	{
		OwnerPlayerController = Cast<APlayerController>(Controller);
		if (OwnerPlayerController == nullptr)
		{
			Deactivate();
		}
		else
		{
			QueryParams.AddIgnoredActor(OwnerPawn.Get());
			QueryParams.bTraceComplex = true;

			FindGlobalPostProcess();
		}
	}
}

void ULimenDynamicDepthOfFieldComponent::TickComponent(float DeltaTime, ELevelTick TickType,
													   FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!IsActive())
	{
		return;
	}

	const float Distance = GetLookAtDistance();
	if (Distance == -1)
	{
		GlobalPostProcess->Settings.DepthOfFieldScale = 0;
	}
	else
	{
		GlobalPostProcess->Settings.DepthOfFieldScale = 1;
		GlobalPostProcess->Settings.DepthOfFieldFocalDistance = Distance;
	}
}

void ULimenDynamicDepthOfFieldComponent::Activate(bool bReset)
{
	if (!HasFoundGlobalPostProcess())
	{
		FindGlobalPostProcess();
	}
	else
	{
		Super::Activate(bReset);
	}
}

void ULimenDynamicDepthOfFieldComponent::FindGlobalPostProcess()
{
	FRecurrentActionDelegate Action;
	Action.BindDynamic(this, &ThisClass::SetPostProcess);

	FRecurrentActionStopCondition StopCondition;
	StopCondition.BindDynamic(this, &ThisClass::HasFoundGlobalPostProcess);
	
	FindGlobalPostProcessAction = ULimenRecurrentAction::StartRecurrentAction(this, Action, 100, .1, StopCondition);
	FindGlobalPostProcessAction->OnSuccess.AddUniqueDynamic(this, &ThisClass::GlobalPostProcessFound);
	FindGlobalPostProcessAction->Activate();
}

void ULimenDynamicDepthOfFieldComponent::GlobalPostProcessFound()
{
	if (bAutoActivate)
	{
		Activate(true);
	}
}

float ULimenDynamicDepthOfFieldComponent::GetLookAtDistance() const
{
	check(OwnerPlayerController != nullptr);

	FVector EyesLocation;
	FRotator EyesRotation;
	OwnerPlayerController->GetActorEyesViewPoint(EyesLocation, EyesRotation);

	FHitResult Hit;
	if (!GetWorld()->LineTraceSingleByChannel(Hit, EyesLocation, EyesLocation + EyesRotation.Vector() * 50000, LineTraceChannel, QueryParams))
	{
		return -1;
	}

	return Hit.Distance;
}

void ULimenDynamicDepthOfFieldComponent::SetPostProcess()
{
	for (TActorIterator<APostProcessVolume> It(GetWorld()); It; ++It)
	{
		if (It->Tags.Contains(GlobalPostProcessTag))
		{
			GlobalPostProcess = *It;
			return;
		}
	}
}

bool ULimenDynamicDepthOfFieldComponent::HasFoundGlobalPostProcess()
{
	return GlobalPostProcess != nullptr;
}
