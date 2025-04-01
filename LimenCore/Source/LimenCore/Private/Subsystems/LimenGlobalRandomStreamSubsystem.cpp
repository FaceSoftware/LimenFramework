// Copyright Face Software. All Rights Reserved.


#include "Subsystems/LimenGlobalRandomStreamSubsystem.h"


void ULimenGlobalRandomStreamSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	int32 Seed;
#if WITH_EDITOR
	Seed = 0;
#else
	Seed = FDateTime::UtcNow().ToUnixTimestamp() ^ FPlatformTime::Cycles();
#endif

	FScopeLock Lock(&RandomStreamSection);
	GlobalRandomStream = MakeShared<FRandomStream, ESPMode::NotThreadSafe>(Seed);
}

void ULimenGlobalRandomStreamSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void ULimenGlobalRandomStreamSubsystem::SetSeed(const int32 Seed)
{
	FScopeLock Lock(&RandomStreamSection);
	GlobalRandomStream->Initialize(Seed);
}

void ULimenGlobalRandomStreamSubsystem::SetSeedWithCurrentTime()
{
	FScopeLock Lock(&RandomStreamSection);
	GlobalRandomStream->Initialize(NAME_None);
}

FRandomStreamRef ULimenGlobalRandomStreamSubsystem::GetGlobalRandomStream()
{
	FScopeLock Lock(&RandomStreamSection);
	return GlobalRandomStream.ToSharedRef();
}

float ULimenGlobalRandomStreamSubsystem::RandomFloat()
{
	FScopeLock Lock(&RandomStreamSection);
	return GlobalRandomStream->FRand();
}

int ULimenGlobalRandomStreamSubsystem::RandomIntRange(const int32 Max, const int32 Min)
{
	check(Max >= Min);
	FScopeLock Lock(&RandomStreamSection);
	return GlobalRandomStream->RandRange(Min, Max);
}

float ULimenGlobalRandomStreamSubsystem::RandomFloatRange(const float Max, const float Min)
{
	check(Max >= Min);
	FScopeLock Lock(&RandomStreamSection);
	return GlobalRandomStream->FRandRange(Min, Max);
}

TArray<int> ULimenGlobalRandomStreamSubsystem::GenerateRandomUniqueNumbers(const int32 Max, const int32 Min, const int32 Count)
{
	check(Max >= Min);
	return GenerateValidRandomUniqueNumbers(Max, Min, Count);
}

TArray<int> ULimenGlobalRandomStreamSubsystem::GenerateRandomNumbers(const int32 Max, const int32 Min, const int32 Count)
{
	check(Max >= Min);
	return GenerateValidRandomNumbers(Max, Min, Count);
}

TArray<int> ULimenGlobalRandomStreamSubsystem::GenerateValidRandomUniqueNumbers(const int32 Max, const int32 Min, const int32 Count, const TFunction<bool(const int32)>& IsNumberValid)
{
	check(Max >= Min);
	checkf(Count <= FMath::Abs(Max - Min) + 1, TEXT("Requested number count too big for the given range"));

	TArray<int> OutIndexes;
	OutIndexes.Reserve(Count);
	
	if (Max == Min)
	{
		if (IsNumberValid(Max))
		{
			OutIndexes.Push(Max);
		}
		
		return OutIndexes;
	}
	
	for (int i = 0; i < Count; i++)
	{
		bool bIsDuplicateIndex = false;
		const uint64 TempIndex = GlobalRandomStream->RandRange(Min, Max);

		for (const auto& Index : OutIndexes)
		{
			if (TempIndex == Index)
			{
				bIsDuplicateIndex = true;
				break;
			}
		}
		
		if (bIsDuplicateIndex || !IsNumberValid(TempIndex))
		{
			i--;
			continue;
		}

		OutIndexes.Push(TempIndex);
	}

	return OutIndexes;
}

TArray<int> ULimenGlobalRandomStreamSubsystem::GenerateValidRandomNumbers(const int32 Max, const int32 Min, const int32 Count, const TFunction<bool(const int32)>& IsIndexValid)
{
	check(Max >= Min);

	TArray<int> OutIndexes;
	OutIndexes.Reserve(Count);
	
	if (Max == Min)
	{
		if (IsIndexValid(Max))
		{
			OutIndexes.Push(Max);
		}
		
		return OutIndexes;
	}
	
	for (int i = 0; i < Count; i++)
	{
		const uint64 TempIndex = GlobalRandomStream->RandRange(Min, Max);		
		if (!IsIndexValid(TempIndex))
		{
			i--;
			continue;
		}

		OutIndexes.Push(TempIndex);
	}

	return OutIndexes;
}
