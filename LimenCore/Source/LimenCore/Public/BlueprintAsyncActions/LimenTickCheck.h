// Copyright © 2024 FaceSoftware. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/World.h"

/**
 * 
 */
class LIMENCORE_API FLimenTickCheck
{
public:
	FLimenTickCheck();
	explicit FLimenTickCheck(UWorld* InWorld, const int32 InMaxTicks = 100);
	~FLimenTickCheck();

	void AddLambda(const TFunction<bool()>& InFunc);

private:
	UWorld* World;
	int32 MaxTicks;
	int32 CurrentTicks;
	FTimerHandle TimerHandle;

	TFunction<bool()> Callback;
	
	void InternalCallback();
};
