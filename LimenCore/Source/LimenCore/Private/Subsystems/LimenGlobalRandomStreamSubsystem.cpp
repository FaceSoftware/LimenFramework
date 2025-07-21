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

int32 ULimenGlobalRandomStreamSubsystem::RandomIntRange(const int32 RangeStart, const int32 RangeEnd)
{
	FScopeLock Lock(&RandomStreamSection);

	const TRange Range(RangeStart, RangeEnd); 
	return GlobalRandomStream->RandRange(Range.Min, Range.Max);
}

float ULimenGlobalRandomStreamSubsystem::RandomFloatRange(const float RangeStart, const float RangeEnd)
{
	FScopeLock Lock(&RandomStreamSection);

	const TRange Range(RangeStart, RangeEnd); 
	return GlobalRandomStream->FRandRange(Range.Min, Range.Max);
}

TArray<int32> ULimenGlobalRandomStreamSubsystem::GenerateRandomUniqueNumbers(const int32 RangeStart, const int32 RangeEnd, const int32 Count)
{
	const TRange Range(RangeStart, RangeEnd); 
	return GenerateValidRandomUniqueNumbers(Range.Min, Range.Max, Count);
}

TArray<int32> ULimenGlobalRandomStreamSubsystem::GenerateRandomNumbers(const int32 RangeStart, const int32 RangeEnd, const int32 Count)
{
	const TRange Range(RangeStart, RangeEnd);
	return GenerateValidRandomNumbers(Range.Min, Range.Max, Count);
}

TArray<int32> ULimenGlobalRandomStreamSubsystem::GenerateValidRandomUniqueNumbers(const int32 RangeStart, const int32 RangeEnd, const int32 Count, const TFunction<bool(const int32)>& IsNumberValid)
{
	const TRange Range(RangeStart, RangeEnd);
	checkf(Count <= FMath::Abs(Range.Max - Range.Min) + 1, TEXT("Requested number count too big for the given range"));

	TArray<int> OutIndexes;
	OutIndexes.Reserve(Count);
	
	if (Range.Max == Range.Min)
	{
		if (IsNumberValid(Range.Max))
		{
			OutIndexes.Push(Range.Max);
		}
		
		return OutIndexes;
	}
	
	for (int i = 0; i < Count; i++)
	{
		bool bIsDuplicateIndex = false;
		const uint64 TempIndex = GlobalRandomStream->RandRange(Range.Min, Range.Max);

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

TArray<int32> ULimenGlobalRandomStreamSubsystem::GenerateValidRandomNumbers(const int32 RangeStart, const int32 RangeEnd, const int32 Count, const TFunction<bool(const int32)>& IsNumberValid)
{
	TArray<int> OutNumbers;
	OutNumbers.Reserve(Count);
	
	for (int i = 0; i < Count; i++)
	{
		const TArray<int32> Number = GenerateValidRandomUniqueNumbers(RangeStart, RangeEnd, Count, IsNumberValid);
		if (!Number.IsEmpty()) OutNumbers.Push(Number[i]);
	}

	return OutNumbers;
}
