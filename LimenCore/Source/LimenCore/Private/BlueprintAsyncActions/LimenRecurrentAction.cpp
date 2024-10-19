// Copyright Face Software. All Rights Reserved.


#include "BlueprintAsyncActions/LimenRecurrentAction.h"

#include "BlueprintLibraries/LimenCoreStatics.h"


ULimenRecurrentAction::ULimenRecurrentAction()
{
	MaxRetries = 0;
	CurrentRetries = 0;
	TimeBetweenAttempts = 0;
	TimePassed = 0;
	TotalTimePassed = 0;
}

void ULimenRecurrentAction::BeginDestroy()
{
	RecurrentAction.Unbind();
	StopCondition.Unbind();
	OnSuccess.Clear();
	OnFailure.Clear();
	FWorldDelegates::OnWorldTickStart.RemoveAll(this);
	
	Super::BeginDestroy();
}

void ULimenRecurrentAction::Activate()
{
	Super::Activate();

	check(CurrentRetries == 0);
	check(StopCondition.IsBound());
	

	FWorldDelegates::OnWorldBeginTearDown.AddLambda([this] (UWorld*)
	{
		FWorldDelegates::OnWorldTickStart.RemoveAll(this);
	});
	
	FWorldDelegates::OnWorldTickStart.AddUObject(this, &ThisClass::WorldTickStart);
}

ULimenRecurrentAction* ULimenRecurrentAction::StartRecurrentAction(UObject* WorldContextObject,
																   const FRecurrentActionDelegate& InRecurrentAction,
																   const int32 InMaxRetries, const float InTimeBetweenAttempts,
																   const FRecurrentActionStopCondition& InStopCondition)
{
	ULimenRecurrentAction* NewAction = NewObject<ULimenRecurrentAction>(WorldContextObject);
	NewAction->World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::Assert);
	NewAction->RecurrentAction = InRecurrentAction;
	NewAction->MaxRetries = InMaxRetries;
	NewAction->CurrentRetries = 0;
	NewAction->TimeBetweenAttempts = InTimeBetweenAttempts;
	NewAction->StopCondition = InStopCondition;
	return NewAction;
}

ULimenRecurrentAction* ULimenRecurrentAction::StartRecurrentAction(UWorld* Outer,
	const FRecurrentActionDelegate& InRecurrentAction, const int32 InMaxRetries, const float InTimeBetweenAttempts,
	const FRecurrentActionStopCondition& InStopCondition)
{
	ULimenRecurrentAction* NewAction = NewObject<ULimenRecurrentAction>(Outer);
	NewAction->World = Outer;
	NewAction->RecurrentAction = InRecurrentAction;
	NewAction->MaxRetries = InMaxRetries;
	NewAction->CurrentRetries = 0;
	NewAction->TimeBetweenAttempts = InTimeBetweenAttempts;
	NewAction->StopCondition = InStopCondition;
	return NewAction;
}

void ULimenRecurrentAction::Action_Internal()
{
	if (RecurrentAction.IsBound())
	{
		RecurrentAction.Execute();
	}
	
	if (StopCondition.IsBound())
	{
		if (StopCondition.Execute())
		{
			if (OnSuccess.IsBound())
			{
				OnSuccess.Broadcast();
			}

			RecurrentAction.Unbind();
			StopCondition.Unbind();
			OnSuccess.Clear();
			OnFailure.Clear();
			FWorldDelegates::OnWorldTickStart.RemoveAll(this);
			ULimenCoreStatics::LimenLog(this, TEXT("Stopping action: Finished successfully..."), ELogType::Log, false);
			return;
		}
	}
	
	if (CurrentRetries >= MaxRetries && MaxRetries != 0)
	{		
		if (OnFailure.IsBound())
		{
			OnFailure.Broadcast();
		}

		RecurrentAction.Unbind();
		StopCondition.Unbind();
		OnSuccess.Clear();
		OnFailure.Clear();
		FWorldDelegates::OnWorldTickStart.RemoveAll(this);
		ULimenCoreStatics::LimenLog(this, TEXT("Stopping action: Finished unsuccessfully..."), ELogType::Warning, false);
		return;
	}
			
	CurrentRetries++;
}

void ULimenRecurrentAction::WorldTickStart(UWorld* InWorld, ELevelTick LevelTick, float DeltaSeconds)
{
	TotalTimePassed += DeltaSeconds;

	if (TotalTimePassed >= Timeout)
	{
		RecurrentAction.Unbind();
		StopCondition.Unbind();
		OnSuccess.Clear();
		OnFailure.Clear();
		FWorldDelegates::OnWorldTickStart.RemoveAll(this);
		ULimenCoreStatics::LimenLog(this, TEXT("Stopping action: Timed out after %d seconds..."), ELogType::Warning, false);
		return;
	}
	
	if (!InWorld->IsPaused())
	{
		TimePassed += DeltaSeconds;
		if (TimePassed > TimeBetweenAttempts)
		{
			Action_Internal();
			TimePassed = 0;
		}
	}
}
