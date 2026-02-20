// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "LimenMath.generated.h"


struct FCollisionQueryParams;
struct FHitResult;
struct FCollisionResponseParams;
struct FConeData;

UENUM(BlueprintType)
enum class EDirection3D : uint8
{
	None		= 0u,
	Forward		= 1u,
	Backward	= 2u,
	Left		= 3u,
	Right		= 4u,
	Up			= 5u,
	Down		= 6u,

	MAX UMETA(Hidden),
};

FORCEINLINE static uint8 DirectionAsInt(const EDirection3D Direction)
{
	return static_cast<uint8>(Direction);
}

FORCEINLINE static EDirection3D GetOppositeDirection(const EDirection3D Direction)
{
	const uint8 DirectionValue = DirectionAsInt(Direction);
	const bool bIsEven = DirectionValue % 2u == 0.f;
	const uint8 Result = bIsEven ? DirectionValue - 1 : DirectionValue + 1;
	return static_cast<EDirection3D>(Result);
}

FORCEINLINE static EDirection3D GetDirectionFromOrientation(const FRotator& Orientation)
{
	const FVector Direction = Orientation.Vector();
	
	EDirection3D Result = EDirection3D::None;
	if (Direction == FVector::ForwardVector)
	{
		Result = EDirection3D::Forward;
	}
	else if (Direction == FVector::BackwardVector)
	{
		Result = EDirection3D::Backward;
	}
	else if (Direction == FVector::RightVector)
	{
		Result = EDirection3D::Right;
	}
	else if (Direction == FVector::LeftVector)
	{
		Result = EDirection3D::Left;
	}
	else if (Direction == FVector::UpVector)
	{
		Result = EDirection3D::Up;
	}
	else if (Direction == FVector::DownVector)
	{
		Result = EDirection3D::Down;
	}
	
	return Result;
}

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
	static T WrappedClamp(T Value, T Min, T Max)
	{
		static_assert(TIsArithmetic<T>());
		check(Min <= Max);
	
		T Range = Max - Min;
		return Min + ((Value - Min) % Range + Range) % Range;
	}
	
	/**
	 * @brief Clamp a rotator to the nearest major axis (10deg = 0deg; 150deg == 180deg).
	 * @param InRotator 
	 * @return 
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Limen|Math Library")
	static FRotator ClampToMajorAxis(const FRotator InRotator);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Limen|Math Library")
	static float QuantizeWithHysteresis(float AngleDegrees, float StepDegrees, float HysteresisDegrees, float& LastSnapDegrees);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Limen|Math Library")
	static float SignedAngleAroundAxis(const FVector& From, const FVector& To, const FVector& Axis);
};


namespace FLimenMath
{
	FORCEINLINE bool IsNearlyEqualByULP(const FVector& A, const FVector& B, int32 Units = 4)
	{
		return FMath::IsNearlyEqualByULP(A.X, B.X, Units) &&
			   FMath::IsNearlyEqualByULP(A.Y, B.Y, Units) &&
			   FMath::IsNearlyEqualByULP(A.Z, B.Z, Units);
	}
	FORCEINLINE bool IsNearlyEqualByULP(const FQuat& A, const FQuat& B, int32 Units = 4)
	{
		return A.Equals(B, FMath::Pow(10.f, static_cast<float>(-Units)));
	}
	FORCEINLINE bool IsNearlyEqualByULP(const FRotator& A, const FRotator& B, int32 Units = 4)
	{
		return FMath::IsNearlyEqualByULP(A.Roll, B.Roll, Units) &&
			   FMath::IsNearlyEqualByULP(A.Pitch, B.Pitch, Units) &&
			   FMath::IsNearlyEqualByULP(A.Yaw, B.Yaw, Units);
	}
}
