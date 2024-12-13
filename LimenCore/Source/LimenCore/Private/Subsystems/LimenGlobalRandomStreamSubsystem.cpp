// Copyright Face Software. All Rights Reserved.


#include "Subsystems/LimenGlobalRandomStreamSubsystem.h"


void ULimenGlobalRandomStreamSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	int32 Seed;
#if WITH_EDITOR
	Seed = 0;
#else
	FDateTime Now = FDateTime::Now();
	Seed = Now.GetSecond() * Now.GetMinute() * Now.GetHour() * Now.GetDayOfYear();
#endif

	FScopeLock Lock(&RandomStreamSection);
	GlobalRandomStream = MakeShared<FRandomStream>(Seed);
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

TSharedRef<FRandomStream> ULimenGlobalRandomStreamSubsystem::GetGlobalRandomStream()
{
	FScopeLock Lock(&RandomStreamSection);
	return GlobalRandomStream.ToSharedRef();
}

float ULimenGlobalRandomStreamSubsystem::RandomFloat()
{
	FScopeLock Lock(&RandomStreamSection);
	return GlobalRandomStream->FRand();
}

int ULimenGlobalRandomStreamSubsystem::RandomIntRange(const int Max, const int Min)
{
	FScopeLock Lock(&RandomStreamSection);
	return GlobalRandomStream->RandRange(Min, Max);
}

float ULimenGlobalRandomStreamSubsystem::RandomFloatRange(const float Max, const float Min)
{
	FScopeLock Lock(&RandomStreamSection);
	return GlobalRandomStream->FRandRange(Min, Max);
}

TArray<int> ULimenGlobalRandomStreamSubsystem::GenerateRandomUniqueNumbers(const int& Max, const int& Min, const int Count)
{
	return GenerateValidRandomUniqueNumbers(Max, Min, Count);
}

TArray<int> ULimenGlobalRandomStreamSubsystem::GenerateRandomNumbers(const int& Max, const int& Min, const int Count)
{
	return GenerateValidRandomNumbers(Max, Min, Count);
}

TArray<int> ULimenGlobalRandomStreamSubsystem::GenerateValidRandomUniqueNumbers(const int& Max, const int& Min, const int Count, const TFunction<bool(const int&)>& IsNumberValid)
{
	ensureMsgf(Count <= FMath::Abs(Max - Min) + 1, TEXT("Requested number count too big for the given range"));

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

TArray<int> ULimenGlobalRandomStreamSubsystem::GenerateValidRandomNumbers(const int& Max, const int& Min, const int Count, const TFunction<bool(const int&)>& IsIndexValid)
{
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
