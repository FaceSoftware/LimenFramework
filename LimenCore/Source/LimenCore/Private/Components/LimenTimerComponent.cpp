// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/LimenTimerComponent.h"


ULimenTimerComponent::ULimenTimerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void ULimenTimerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void ULimenTimerComponent::BeginPlay()
{
	Super::BeginPlay();

	Start = InvalidTime;
	End = InvalidTime;
}

void ULimenTimerComponent::ResetTimer()
{
	check(GetOwner()->HasAuthority())

	Multicast_SetStart(InvalidTime.GetTicks());
	Multicast_SetEnd(InvalidTime.GetTicks());
}

void ULimenTimerComponent::StartTimer()
{
	check(GetOwner()->HasAuthority())
	ResetTimer();

	Multicast_SetStart(FDateTime::Now().GetTicks());
}

void ULimenTimerComponent::StopTimer()
{
	check(GetOwner()->HasAuthority())

	Multicast_SetEnd(FDateTime::Now().GetTicks());
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

void ULimenTimerComponent::Multicast_SetStart_Implementation(const int64 InStart)
{
	Start = FDateTime(InStart);
	if (Start != InvalidTime) OnStart.Broadcast(this);
}

void ULimenTimerComponent::Multicast_SetEnd_Implementation(const int64 InEnd)
{
	End = FDateTime(InEnd);
	if (End != InvalidTime) { OnStop.Broadcast(this); }
}
