// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "Subsystems/EngineSubsystem.h"
#include "LimenGlobalRandomStreamSubsystem.generated.h"


typedef TSharedRef<FRandomStream, ESPMode::NotThreadSafe> FRandomStreamRef;
typedef TSharedPtr<FRandomStream, ESPMode::NotThreadSafe> FRandomStreamPtr;

/**
 * 
 */
UCLASS()
class LIMENGLOBALRANDOMSTREAM_API ULimenGlobalRandomStreamSubsystem : public UEngineSubsystem
{
	GENERATED_BODY()

public:
	static ULimenGlobalRandomStreamSubsystem* Get()
	{
		return GEngine->GetEngineSubsystem<ULimenGlobalRandomStreamSubsystem>();
	}

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable)
	void SetSeed(const int32 Seed);

	UFUNCTION(BlueprintCallable)
	void SetSeedWithCurrentTime();
	
	FRandomStreamRef GetGlobalRandomStream();
	
	float RandomFloat();


	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetFraction();
	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 RandomIntRange(const int32 RangeStart, const int32 RangeEnd);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	float RandomFloatRange(const float RangeStart, const float RangeEnd);
	
	UFUNCTION(BlueprintCallable)
	TArray<int32> GenerateRandomUniqueNumbers(const int32 RangeStart, const int32 RangeEnd, const int32 Count);

	UFUNCTION(BlueprintCallable)
	TArray<int32> GenerateRandomNumbers(const int32 RangeStart, const int32 RangeEnd, const int32 Count);
	
	TArray<int32> GenerateValidRandomUniqueNumbers(const int32 RangeStart, const int32 RangeEnd, const int32 Count, const TFunction<bool(const int32)>& IsNumberValid = [] (const int32) { return true; });

	TArray<int32> GenerateValidRandomNumbers(const int32 RangeStart, const int32 RangeEnd, const int32 Count, const TFunction<bool(const int32)>& IsNumberValid = [] (const int32) { return true; });
	
protected:
	
private:
	FRandomStreamPtr GlobalRandomStream;
	FCriticalSection RandomStreamSection;


	template <typename T>
	struct  TRange
	{
		static_assert(TIsArithmetic<T>::Value, "T must be an arithmetic type");
		
		TRange(T RangeStart, T RangeEnd)
		{
			Min = FMath::Min(RangeStart, RangeEnd);
			Max = FMath::Max(RangeStart, RangeEnd);
		}

		T Min;
		T Max;
	};
};
