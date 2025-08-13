// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "LimenMath.generated.h"


struct FCollisionQueryParams;
struct FHitResult;
struct FCollisionResponseParams;
struct FConeData;
/**
 * 
 */
UCLASS()
class LIMENCORE_API ULimenMath : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * @brief Rounds a floating point number to the nearest integer value (2.5 == 3 and -1.1 == -1)
	 * @param Value The value to round to.
	 * @return The rounded value.
	 */
	UFUNCTION(BlueprintCallable, Category="Limen|Math Library", BlueprintPure)
	static int64 RoundToNearestInteger(const double Value);

	/**
	 * @brief Rounds a floating point number to the nearest value specified number of decimals.
	 * @param Value The number to round.
	 * @param NumDecimals The number of decimals to round to.
	 * @return The rounded value.
	 * @warning May cause memory overflow when NumDecimals is too high. If numbers do not match try using a lower value for NumDecimals.
	 */
	UFUNCTION(BlueprintCallable, Category="Limen|Math Library", BlueprintPure)
	static double RoundToNearestDouble(const double Value, const uint8 NumDecimals = 2);

	UFUNCTION(BlueprintCallable, Category="Limen|Math Library")
	static void RotateActorAroundPoint(AActor* Actor, const FVector Point, const double DegreeRotation, const FVector Axis = FVector(0.f, 0.f, 1.f));

	UFUNCTION(BlueprintCallable, Category="Limen|Math Library")
	static bool RotatePawnAroundPoint(AController* Controller, const FVector Point, const double DegreeRotation, const FVector Axis = FVector(0.f, 0.f, 1.f));
	
	UFUNCTION(BlueprintCallable, Category="Limen|Math Library")
	static FTransform RotatePointAroundPoint(const FVector Point, const FVector RotationCenter, const double DegreeRotation, const FVector Axis = FVector(0.f, 0.f, 1.f));

	UFUNCTION(BlueprintCallable, Category="Limen|Math Library")
	static FTransform RotateTransform(const FTransform TransformToRotate, const FVector RotationCenter, const double DegreeRotation, const FVector Axis = FVector(0.f, 0.f, 1.f));

	UFUNCTION(BlueprintCallable, Category="Limen|Math Library", meta=(WorldContext="Caller"))
	static void GetRandomReachablePointInCircumference(UObject* Caller, const FVector& Origin, const float Radius, int32 NumOfPoints, FVector& OutLocation, bool& bSuccess);

	UFUNCTION(BlueprintCallable, Category="Limen|Math Library")
	static AActor* GetClosestActorTo(const FVector& Location, const TArray<AActor*>& ActorsList);

	UFUNCTION(BlueprintCallable, Category="Limen|Math Library")
	static AActor* GetClosestActorOfClassTo(const FVector& Location, const TArray<AActor*>& ActorsList, const TSubclassOf<AActor>& InClass);

	UFUNCTION(BlueprintCallable, Category="Limen|Math Library")
	static UPrimitiveComponent* GetClosestComponentsTo(const FVector& Location, const TArray<UPrimitiveComponent*>& ComponentsList, float& OutClosestDistanceSquared);
	
	static bool ConeTraceMultiByProfile(const UWorld* World, const FConeData& InConeData,
											const FName& ProfileName,
											TArray<FHitResult>& OutHits,
											const FCollisionQueryParams& QueryParams,
											const FCollisionResponseParams& ResponseParams,
											const float TraceResolution,
											const bool bThreadSafe = true);

	static bool IsInteger(const double Test);

	template<typename T>
	static T WrappedClamp(T Value, T Min, T Max);
};

template <typename T>
T ULimenMath::WrappedClamp(T Value, T Min, T Max)
{
	static_assert(TIsArithmetic<T>());
	check(Min <= Max);
	
	T Range = Max - Min;
	return Min + ((Value - Min) % Range + Range) % Range;
}
