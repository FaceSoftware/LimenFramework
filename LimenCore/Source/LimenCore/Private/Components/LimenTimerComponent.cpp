// Copyright FaceSoftware. All rights reserved.


#include "Components/LimenTimerComponent.h"

#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"


ULimenTimerComponent::ULimenTimerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void ULimenTimerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;

	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, Start, Params)
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, End, Params)
}

void ULimenTimerComponent::BeginPlay()
{
	Super::BeginPlay();

	ResetTimer();
}

void ULimenTimerComponent::ResetTimer()
{
	check(GetOwner()->HasAuthority())

	Start = End = InvalidTime;
	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, Start, this)
	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, End, this)
}

void ULimenTimerComponent::StartTimer()
{
	check(GetOwner()->HasAuthority())
	ResetTimer();

	Start = FDateTime::Now();
	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, Start, this)

	OnStart.Broadcast(this);
}

void ULimenTimerComponent::StopTimer()
{
	check(GetOwner()->HasAuthority())

	End = FDateTime::Now();
	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, End, this)

	OnStop.Broadcast(this);
}

FTimespan ULimenTimerComponent::GetCurrentTime() const
{
	if (Start == InvalidTime)
	{
		return FTimespan::Zero();
	}
	if (End == InvalidTime)
	{
		return FDateTime::Now() - Start;
	}
	return End - Start;
}

FDateTime ULimenTimerComponent::GetStartTime() const
{
	return Start;
}

bool ULimenTimerComponent::IsTimerActive() const
{
	return Start != InvalidTime && End == InvalidTime;
}

void ULimenTimerComponent::OnRep_Start()
{
	if (Start != InvalidTime) { OnStart.Broadcast(this); }
}

void ULimenTimerComponent::OnRep_End()
{
	if (End != InvalidTime) { OnStop.Broadcast(this); }
}
