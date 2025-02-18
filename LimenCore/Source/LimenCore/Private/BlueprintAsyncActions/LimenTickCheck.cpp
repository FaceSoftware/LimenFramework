// Copyright © 2024 FaceSoftware. All rights reserved.


#include "BlueprintAsyncActions/LimenTickCheck.h"

#include "TimerManager.h"


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
}

void FLimenTickCheck::AddLambda(const TFunction<bool()>& InFunc)
{
	InFunc.CheckCallable();

	if (World == nullptr)
	{
		return;
	}

	Callback = InFunc;
	InternalCallback();
}

void FLimenTickCheck::InternalCallback()
{
	if (!IsValid(World) || !World->IsGameWorld() || !Callback)
	{
		return;
	}
	
	if (Callback())
	{
		Callback.Reset();
		return;
	}

	if (++CurrentTicks >= MaxTicks)
	{
		// checkNoEntry()
	}
	else
	{
		const FTimerDelegate Delegate = FTimerDelegate::CreateRaw(this, &FLimenTickCheck::InternalCallback);
		TimerHandle = World->GetTimerManager().SetTimerForNextTick(Delegate);
	}
}
