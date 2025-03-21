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
class LIMENCORE_API ULimenGlobalRandomStreamSubsystem : public UEngineSubsystem
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
	int RandomIntRange(const int Max, const int Min);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	float RandomFloatRange(const float Max, const float Min);
	
	UFUNCTION(BlueprintCallable)
	TArray<int> GenerateRandomUniqueNumbers(const int& Max, const int& Min, const int Count);

	UFUNCTION(BlueprintCallable)
	TArray<int> GenerateRandomNumbers(const int& Max, const int& Min, const int Count);
	
	TArray<int> GenerateValidRandomUniqueNumbers(const int& Max, const int& Min, const int Count, const TFunction<bool(const int&)>& IsNumberValid = [] (const int&) { return true; });

	TArray<int> GenerateValidRandomNumbers(const int& Max, const int& Min, const int Count, const TFunction<bool(const int&)>& IsIndexValid = [] (const int&) { return true; });
	
protected:
	
private:
	FRandomStreamPtr GlobalRandomStream;

	FCriticalSection RandomStreamSection;
};
