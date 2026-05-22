// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "HAL/RunnableThread.h"
#include "Stats/Stats.h"

namespace ErrorInformation
{
	constexpr static float ErrorDisplayTime = 10.f;
	const static FColor& ErrorTextColor = FColor::Red;
	
	constexpr static float WarningDisplayTime = 10.f;
	const static FColor& WarningTextColor = FColor::Yellow;
	
	constexpr static float LogDisplayTime = 5.f;
	const static FColor& LogTextColor = FColor::Green;
}

namespace LimenThreading::ExitCodes
{
	// Success execution 
	constexpr static uint32 Success = 0;
	
	// When kill is requested
	constexpr static uint32 EarlyStop = 1;

	// Error that the thread cannot recover from
	constexpr static uint32 CriticalError = 2;

	// Non critical error
	constexpr static uint32 NonCriticalError = 3;

	// When something made the operation redundant
	constexpr static uint32 RedundancyWarning = 4;
}

namespace LimenThreading
{
	inline FString ConvertExitCodeToString(const FRunnableThread* Thread, const uint8 ExitCode)
	{
		return FString::Printf(TEXT("Thread with name %s returned with exit code %d. See LimenThreading::ExitCodes for details on the error"), *Thread->GetThreadName(), ExitCode);	
	}
}

DECLARE_STATS_GROUP(TEXT("Vector Maths"), STATGROUP_VectorMaths, STATCAT_Advanced);
DECLARE_CYCLE_STAT(TEXT("ConeTraceMultiByChannel"), STAT_ConeTraceMultiByChannel, STATGROUP_VectorMaths);
