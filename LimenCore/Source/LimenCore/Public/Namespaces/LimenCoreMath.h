// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LimenCoreMath.generated.h"

/**
 * 
 */

struct FConeData
{
	FConeData() = default;
	FConeData(const FConeData& Other) = default;
	
	static FConeData FromAngle(const FVector& InApex, const FQuat& InOrientation, const float InAngle, const float InHeight)
	{
		FConeData Out;
		Out.Apex = InApex;
		Out.Orientation = InOrientation;
		Out.Angle = InAngle;
		Out.Height = InHeight;
		return Out;
	}
	
	~FConeData() = default;

	bool IsPossible() const
	{
		return !Apex.IsNearlyZero() && !FMath::IsNearlyZero(Angle) || !FMath::IsNearlyZero(Height);
	}
	
	// The apex point of the cone
	const FVector& GetApex() const { return Apex; }
	// The orientation of the cone
	const FQuat& GetOrientation() const { return Orientation; }
	// The full angle of the cone
	float GetAngle() const { return Angle; }
	// The height of the cone
	float GetHeight() const { return Height; }
	// The base center point. Lazy evaluation
	const FVector& GetBaseCenterPoint() const
	{
		if (!BaseCenterPoint.IsSet())
		{
			BaseCenterPoint = Apex + Orientation.GetRotationAxis().Normalize() * Height;
		}

		return BaseCenterPoint.GetValue();
	}
	FVector GetSafeBaseCenterPoint() const
	{
		return Apex + Orientation.GetRotationAxis().Normalize() * Height;
	}
	
	// The length of the cone's hypotenuse. Lazy evaluation
	float GetHypotenuseLength() const
	{
		if (!HypotenuseLength.IsSet())
		{
			HypotenuseLength = GetHeight() * FMath::Tan(FMath::DegreesToRadians(GetAngle() / 2));
			// HypotenuseLength = FMath::Sqrt(FMath::Square(GetBaseRadius()) + FMath::Square(GetHeight()));
		}

		return HypotenuseLength.GetValue();
	}
	// The cone's volume. Lazy evaluation
	float GetVolume() const
	{
		if (!Volume.IsSet())
		{
			Volume = (1 / 3) * PI * FMath::Square(GetBaseRadius()) * Height;
		}

		return Volume.GetValue();
	}
	// The base radius. Lazy evaluation
	float GetBaseRadius() const
	{
		if (!BaseRadius.IsSet())
		{
			BaseRadius = GetRadiusAtHeight(Height);
		}

		return BaseRadius.GetValue();	
	}
	float GetRadiusAtHeight(const float TestHeight) const
	{
		return TestHeight * FMath::Tan(FMath::DegreesToRadians(Angle / 2));
	}
	

private:
	// The apex point of the cone
	FVector Apex;
	// The orientation of the cone
	FQuat Orientation;
	// The full angle of the cone
	float Angle;
	// The height of the cone
	float Height;
	// The base center point
	mutable TOptional<FVector> BaseCenterPoint;
	// The length of the cone's hypotenuse
	mutable TOptional<float> HypotenuseLength;
	// The cone's volume
	mutable TOptional<float> Volume;
	// The base radius
	mutable TOptional<float> BaseRadius;

	mutable TOptional<FVector2D> LocalHypotenuseBasePoint;
	
	const FVector2D& GetLocalHypotenusePoint() const
	{
		if (!LocalHypotenuseBasePoint.IsSet())
		{
			FVector2D HypotenusePoint;
			HypotenusePoint.Y = FMath::Tan(FMath::DegreesToRadians(GetAngle() / 2));
			HypotenusePoint.X = Height;

			LocalHypotenuseBasePoint = HypotenusePoint;
		}

		return LocalHypotenuseBasePoint.GetValue();
	}
};



USTRUCT(BlueprintType)
struct FLimenStringNumber
{
	GENERATED_BODY()

public:
	FLimenStringNumber();
	FLimenStringNumber(const FString& InStr);
	FLimenStringNumber(const FLimenStringNumber& Other);

	template<typename T, typename = typename TEnableIf<TIsArithmetic<T>::Value>::Type>
	FLimenStringNumber(T InValue) { FromString(FString::Printf(TEXT("%.15g"), InValue)); }

	// Arithmetic
	FLimenStringNumber operator+(const FLimenStringNumber& Other) const;
	FLimenStringNumber operator-(const FLimenStringNumber& Other) const;
	FLimenStringNumber operator*(const FLimenStringNumber& Other) const;
	FLimenStringNumber operator/(const FLimenStringNumber& Other) const;

	// Comparison
	bool operator==(const FLimenStringNumber& Other) const;
	bool operator!=(const FLimenStringNumber& Other) const;
	bool operator<(const FLimenStringNumber& Other) const;
	bool operator>(const FLimenStringNumber& Other) const;

	FString ToString() const;

private:
	FString IntegerPart = TEXT("0");
	FString DecimalPart = TEXT("");
	bool bIsNegative = false;

	void FromString(const FString& InStr);
	void Normalize();

	// Internal utilities
	static FString AddStrings(const FString& A, const FString& B);
	static FString SubtractStrings(const FString& A, const FString& B);
	static FString MultiplyStrings(const FString& A, const FString& B);
	static FString DivideStrings(const FString& A, const FString& B, int32 Precision = 10);

	static bool IsLessThan(const FString& A, const FString& B);
	static void AlignDecimals(FString& ADec, FString& BDec);
	static FString TrimLeadingZeros(const FString& Str);
	static FString TrimTrailingZeros(const FString& Str);
};