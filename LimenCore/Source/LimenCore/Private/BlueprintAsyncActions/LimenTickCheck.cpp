// Copyright © 2024 FaceSoftware. All rights reserved.


#include "BlueprintAsyncActions/LimenTickCheck.h"


FLimenTickCheck::FLimenTickCheck()
{
	World = nullptr;
	MaxTicks = 0;
	CurrentTicks = 0;
}

FLimenTickCheck::FLimenTickCheck(UWorld* InWorld, const int32 InMaxTicks)
	: World(InWorld), MaxTicks(InMaxTicks), CurrentTicks(0)
{
}

FLimenTickCheck::~FLimenTickCheck()
{
	if (IsValid(World))
	{
		World->GetTimerManager().ClearTimer(TimerHandle);
	}
}

void FLimenTickCheck::AddLambda(const TFunction<bool()>& InFunc)
{
	InFunc.CheckCallable();

	if (World == nullptr)
	{
		return;
	}
	
	InternalCallback(InFunc);
}

void FLimenTickCheck::InternalCallback(const TFunction<bool()> InFunctor)
{
	if (World == nullptr)
	{
		return;
	}

	check(InFunctor != nullptr)
	
	if (InFunctor())
	{
		if (World != nullptr)
		{
			World->GetTimerManager().ClearTimer(TimerHandle);
		}
		return;
	}

	if (++CurrentTicks >= MaxTicks)
	{
		checkNoEntry()
	}
	else
	{
		TimerHandle = World->GetTimerManager().SetTimerForNextTick([this, InFunctor]
		{
			InternalCallback(InFunctor);
		});
	}
}
