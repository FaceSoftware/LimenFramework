// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

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

	/* static FConeData FromRadius(const FVector& InApex, const FQuat& InOrientation, const float InBaseRadius, const float InHeight)
	{
		FConeData Out;
		Out.Apex = InApex;
		Out.Orientation = InOrientation;
		Out.Height = InHeight;
		Out.BaseRadius = InBaseRadius;
		return Out;
	} */
	
	~FConeData() = default;

	bool IsPossible() const
	{
		return Apex.IsNearlyZero() || FMath::IsNearlyZero(Angle) || FMath::IsNearlyZero(Height); 
	}
	
	// The apex point of the cone
	const FVector& GetApex() { return Apex; }
	// The orientation of the cone
	const FQuat& GetOrientation() { return Orientation; }
	// The full angle of the cone
	const float& GetAngle() { return Angle; }
	// The height of the cone
	const float& GetHeight() { return Height; }
	// The base center point. Lazy evaluation
	const FVector& GetBaseCenterPoint()
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
	const float& GetHypotenuseLength()
	{
		if (!HypotenuseLength.IsSet())
		{
			HypotenuseLength = GetHeight() * FMath::Tan(FMath::DegreesToRadians(GetAngle() / 2));
			// HypotenuseLength = FMath::Sqrt(FMath::Square(GetBaseRadius()) + FMath::Square(GetHeight()));
		}

		return HypotenuseLength.GetValue();
	}
	// The cone's volume. Lazy evaluation
	const float& GetVolume()
	{
		if (!Volume.IsSet())
		{
			Volume = (1 / 3) * PI * FMath::Square(GetBaseRadius()) * Height;
		}

		return Volume.GetValue();
	}
	// The base radius. Lazy evaluation
	const float& GetBaseRadius()
	{
		if (!BaseRadius.IsSet())
		{
			BaseRadius = GetRadiusAtHeight(Height);
		}

		return BaseRadius.GetValue();	
	}
	float GetRadiusAtHeight(const float TestHeight)
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
	TOptional<FVector> BaseCenterPoint;
	// The length of the cone's hypotenuse
	TOptional<float> HypotenuseLength;
	// The cone's volume
	TOptional<float> Volume;
	// The base radius
	TOptional<float> BaseRadius;

	TOptional<FVector2D> LocalHypotenuseBasePoint;
	
	const FVector2D& GetLocalHypotenusePoint()
	{
		if (!LocalHypotenuseBasePoint.IsSet())
		{
			FVector2D HypotenusePoint;
			HypotenusePoint.Y = FMath::Tan(FMath::DegreesToRadians(GetAngle() / 2));
			HypotenusePoint.X = Height;

			TOptional<FVector2D>* NonConstHypotenusePoint = const_cast<TOptional<FVector2D>*>(&LocalHypotenuseBasePoint);
			*NonConstHypotenusePoint = HypotenusePoint;
		}

		return LocalHypotenuseBasePoint.GetValue();
	}
};;
