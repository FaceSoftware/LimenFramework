// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Subsystems/LimenGlobalRandomStreamSubsystem.h"
#include "LimenGlobalRandomStreamUtils.generated.h"

/**
 * 
 */
UCLASS()
class LIMENGLOBALRANDOMSTREAM_API ULimenGlobalRandomStreamUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * @brief Acts as a coin toss
	 * @param ChanceToWin The probability of winning.
	 * @param bWon True if won, false if lost
	 */
	UFUNCTION(BlueprintCallable, Category="Limen|Global Random Stream Utils", meta=(WorldContext="WorldContext", ExpandBoolAsExecs="bWon"))
	static void RandomPercentage(const double ChanceToWin, UPARAM(DisplayName="Has Won")bool& bWon);

	template <typename T>
	static TArray<T> ShuffleArray(const TArray<T>& In)
	{
		TArray<T> Out = In;
		ULimenGlobalRandomStreamSubsystem* RandomStream = ULimenGlobalRandomStreamSubsystem::Get();
	
		if (Out.Num() > 1)
		{
			for (int32 i = 0; i < Out.Num(); i++)
			{
				const int32 j = RandomStream->RandomIntRange(Out.Num() - 1, i);
				if (i != j)
				{
					Out.Swap(i, j);
				}
			}
		}

		return Out;
	}

	
	UFUNCTION(BlueprintCallable, Category="Limen|Global Random Stream Utils", meta=(WorldContext="Caller"))
	static void GetRandomReachablePointInCircumference(UObject* Caller, const FVector& Origin, const float Radius, int32 NumOfPoints, FVector& OutLocation, bool& bSuccess);
};
