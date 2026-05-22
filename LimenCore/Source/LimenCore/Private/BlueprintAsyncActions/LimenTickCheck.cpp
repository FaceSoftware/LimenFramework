// Copyright © 2024 FaceSoftware. All rights reserved.


#include "BlueprintAsyncActions/LimenTickCheck.h"

#include "TimerManager.h"
#include "Engine.h"


FLimenTickCheck::FLimenTickCheck(const int32 InMaxTicks)
	: World(GEngine != nullptr ? GEngine->GetWorld() : nullptr), MaxTicks(FMath::Max(InMaxTicks, MaxPossibleTicks)), CurrentTicks(0)
{
}

FLimenTickCheck::~FLimenTickCheck()
{
}

void FLimenTickCheck::AddLambda(const TFunction<bool()>& InFunc)
{
	check(InFunc);

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
