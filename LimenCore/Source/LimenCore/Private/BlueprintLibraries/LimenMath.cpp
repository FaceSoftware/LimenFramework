// Copyright Face Software. All Rights Reserved.


#include "BlueprintLibraries/LimenMath.h"

#include "DrawDebugHelpers.h"
#include "Components/PrimitiveComponent.h"
#include "GameFramework/Pawn.h"
#include "Namespaces/LimenCoreMath.h"


int64 ULimenMath::RoundToNearestInteger(const double Value)
{
	const int64 Above = static_cast<int64>(Value + 1);
	const double AboveDifference = FMath::Abs(Value - Above);
	const int64 Below = static_cast<int64>(Value);
	const double BelowDifference = FMath::Abs(Value - Below);

	return BelowDifference > AboveDifference ? Above : Below;
}

double ULimenMath::RoundToNearestDouble(const double Value, const uint8 NumDecimals)
{
	if (NumDecimals == 0)
	{
		return RoundToNearestInteger(Value);
	}

	uint64 Factor = 1;
	for (int i = 0; i < NumDecimals; ++i)
	{
		Factor *= 10;
	}

	const uint64 Integer = RoundToNearestInteger(Value * Factor);
	const double Output = Integer / Factor;
	return Output;
}

void ULimenMath::RotateActorAroundPoint(AActor* Actor, const FVector Point, const double DegreeRotation,
										const FVector Axis)
{
	const FVector ActorLocation = Actor->GetActorLocation();
	const FVector Radius = ActorLocation - Point;

	const FVector AxisNormal = Axis.GetSafeNormal();
	const FVector NewLocation = Point + Radius.RotateAngleAxis(DegreeRotation, AxisNormal);

	FRotator NewRotation;
	NewRotation.Yaw = AxisNormal.Z * DegreeRotation;
	NewRotation.Pitch = AxisNormal.Y * DegreeRotation;
	NewRotation.Roll = AxisNormal.X * DegreeRotation;

	Actor->SetActorLocation(NewLocation, false, nullptr, ETeleportType::TeleportPhysics);
	Actor->SetActorRotation(NewRotation, ETeleportType::TeleportPhysics);
}

bool ULimenMath::RotatePawnAroundPoint(AController* Controller, const FVector Point, const double DegreeRotation,
									   const FVector Axis)
{
	// Check if we have a valid controller and a controlled pawn
	if (Controller && Controller->GetPawn())
	{
		return false;
	}

	APawn* Pawn = Controller->GetPawn();

	// Step 1: Create the rotation quaternion based on the axis and the degree of rotation
	const FQuat RotationQuat(Axis.GetSafeNormal(), FMath::DegreesToRadians(DegreeRotation));

	// Step 2: Calculate the vector from the point to the pawn's location
	const FVector PawnOffset = Pawn->GetActorLocation() - Point;

	// Step 3: Rotate the offset vector
	const FVector RotatedOffset = RotationQuat.RotateVector(PawnOffset);

	// Step 4: Calculate the new location by adding the rotated offset back to the point
	const FVector NewLocation = Point + RotatedOffset;

	// Step 5: Calculate the new rotation for the pawn by combining the current rotation with the rotation quaternion
	const FQuat NewPawnRotation = Pawn->GetActorQuat() * RotationQuat;

	// Step 6: Update the pawn's transform
	const FTransform NewTransform(NewPawnRotation, NewLocation);
	Pawn->SetActorTransform(NewTransform);

	// Step 7: Update the control rotation to maintain the pawn's orientation relative to the world
	const FRotator NewControlRotation = NewPawnRotation.Rotator();
	Controller->SetControlRotation(NewControlRotation.GetInverse());


	return true;
}

FTransform ULimenMath::RotatePointAroundPoint(const FVector Point, const FVector RotationCenter,
											  const double DegreeRotation,
											  const FVector Axis)
{
	// Normalize the axis to ensure it's a unit vector
	const FVector NormalizedAxis = Axis.GetSafeNormal();

	// Create a quaternion representing the rotation by the specified degrees around the specified axis
	const FQuat RotationQuat(NormalizedAxis, FMath::DegreesToRadians(DegreeRotation));

	// Translate the point to a space where RotationCenter is the origin
	const FVector TranslatedPoint = Point - RotationCenter;

	// Rotate the translated point around the RotationCenter
	const FVector RotatedPoint = RotationQuat.RotateVector(TranslatedPoint);

	// Translate the rotated point back to the original space
	const FVector NewPosition = RotationCenter + RotatedPoint;

	// Create a new transform with the new position. We're not changing the rotation or scale, so we use the identity quaternion for rotation
	// and a scale of (1,1,1) for no scaling.
	const FTransform NewTransform(FQuat::Identity, NewPosition, FVector(1.f, 1.f, 1.f));

	return NewTransform;
}

FTransform ULimenMath::RotateTransform(const FTransform TransformToRotate, const FVector RotationCenter,
									   const double DegreeRotation, const FVector Axis)
{
	// Normalize the axis to ensure it's a unit vector
	FVector NormalizedAxis = Axis.GetSafeNormal();

	// Create a quaternion representing the rotation by the specified degrees around the specified axis
	FQuat RotationQuat(NormalizedAxis, FMath::DegreesToRadians(DegreeRotation));

	// Translate the transform to a space where RotationCenter is the origin
	FTransform TranslatedTransform = TransformToRotate;
	TranslatedTransform.AddToTranslation(-RotationCenter);

	// Apply the rotation to the translated transform
	FTransform RotatedTransform(RotationQuat * TranslatedTransform.GetRotation(), TranslatedTransform.GetTranslation(),
								TranslatedTransform.GetScale3D());

	// Translate the rotated transform back to the original space
	FTransform NewTransform = RotatedTransform;
	NewTransform.AddToTranslation(RotationCenter);

	return NewTransform;
}

AActor* ULimenMath::GetClosestActorTo(const FVector& Location, const TArray<AActor*>& ActorsList)
{
	double ClosestDistance = TNumericLimits<double>::Max();
	AActor* ClosestActor = nullptr;

	for (AActor* Test : ActorsList)
	{
		const double CurrentDistance = FVector::DistSquared(Location, Test->GetActorLocation());
		if (CurrentDistance < ClosestDistance)
		{
			ClosestDistance = CurrentDistance;
			ClosestActor = Test;
		}
	}

	return ClosestActor;
}

AActor* ULimenMath::GetClosestActorOfClassTo(const FVector& Location, const TArray<AActor*>& ActorsList,
											 const TSubclassOf<AActor>& InClass)
{
	float ClosestDistance = TNumericLimits<double>::Max();
	AActor* ClosestActor = nullptr;

	for (AActor* Test : ActorsList)
	{
		if (!Test->GetClass()->IsChildOf(InClass))
		{
			continue;
		}

		// const float CurrentDistance = FVector::DistSquared(Location, Test->GetActorLocation());
		float CurrentDistance = TNumericLimits<double>::Max();

		TArray<UPrimitiveComponent*> PrimitiveComponents;
		Test->GetComponents<UPrimitiveComponent>(PrimitiveComponents);
		GetClosestComponentsTo(Location, PrimitiveComponents, CurrentDistance);

		if (CurrentDistance < ClosestDistance)
		{
			ClosestDistance = CurrentDistance;
			ClosestActor = Test;
		}
	}

	return ClosestActor;
}

UPrimitiveComponent* ULimenMath::GetClosestComponentsTo(const FVector& Location,
														const TArray<UPrimitiveComponent*>& ComponentsList,
														float& OutClosestDistanceSquared)
{
	double ClosestDistance = TNumericLimits<double>::Max();
	UPrimitiveComponent* ClosestComponent = nullptr;

	for (UPrimitiveComponent* Test : ComponentsList)
	{
		// double CurrentDistance = Test->GetClosestPointOnCollision(Location, ClosestPoint);
		float CurrentDistance = -1.f;
		FVector ClosestPoint;
		const FBodyInstance* BodyInst = Test->GetBodyInstance();
		if (BodyInst == nullptr || !BodyInst->GetSquaredDistanceToBody(Location, CurrentDistance, ClosestPoint))
		{
			CurrentDistance = FVector::DistSquared(Location, Test->GetComponentLocation());
		}

		if (CurrentDistance < ClosestDistance)
		{
			ClosestDistance = CurrentDistance;
			ClosestComponent = Test;
		}
	}

	OutClosestDistanceSquared = ClosestDistance;
	return ClosestComponent;
}

bool ULimenMath::ConeTraceMultiByProfile(const UWorld* World, const FConeData& InConeData,
										 const FName& ProfileName,
										 TArray<FHitResult>& OutHits,
										 const FCollisionQueryParams& QueryParams,
										 const FCollisionResponseParams& ResponseParams,
										 const float TraceResolution, const bool bThreadSafe)
{
	if (World == nullptr)
	{
		return false;
	}

	bool bBlockingHit = false;
	OutHits.Empty();
	const uint32 NumberOfTraces = InConeData.GetHeight() / (1 / TraceResolution);
	const float TracePositionIncrement = InConeData.GetHeight() / NumberOfTraces;

	float CurrentHeight = 0.f;
	FVector CurrentTracePosition = InConeData.GetApex();
	for (uint32 i = 0; i < NumberOfTraces; ++i)
	{
		TArray<FHitResult> TempOutHits;
		const float TempSphereRadius = InConeData.GetRadiusAtHeight(CurrentHeight);

		const FVector ConeBaseCenterLocalPosition = InConeData.GetSafeBaseCenterPoint();
		const FVector EndTrace = CurrentTracePosition + ConeBaseCenterLocalPosition;
		bBlockingHit = World->SweepMultiByProfile(TempOutHits, CurrentTracePosition, EndTrace, FQuat::Identity, ProfileName, FCollisionShape::MakeSphere(InConeData.GetBaseRadius()), QueryParams);
		
		if (!TempOutHits.IsEmpty())
		{
			bool bIsRepeated = false;
			for (const auto& TempOutHit : TempOutHits)
			{
				for (const auto& Hit : OutHits)
				{
					if (Hit.GetActor() == TempOutHit.GetActor())
					{
						bIsRepeated = true;
						break;
					}
				}

				if (!bIsRepeated)
				{
					OutHits.Push(TempOutHit);
				}
			}

#if !(UE_BUILD_TEST || UE_BUILD_SHIPPING)
			if (QueryParams.bDebugQuery && !bThreadSafe)
			{
				FPlane Plane(InConeData.GetOrientation().GetRotationAxis());
				FVector VectorA, VectorB;
				Plane.FindBestAxisVectors(VectorA, VectorB);
				DrawDebugCircle(World, CurrentTracePosition, TempSphereRadius, 64, FColor::Green,
								false, -1, 0, 5.f,
								VectorA, VectorA);
			}
#endif
		}
		
		CurrentHeight += TracePositionIncrement;
		CurrentTracePosition += InConeData.GetOrientation().GetRotationAxis() * TracePositionIncrement;
	}

	return bBlockingHit;
}

bool ULimenMath::IsInteger(const double Test)
{
	return FMath::Abs(Test - FMath::RoundToInt(Test)) < UE_DOUBLE_KINDA_SMALL_NUMBER;
}

FRotator ULimenMath::ClampToMajorAxis(const FRotator InRotator)
{
	const FRotator ClampedRotator = InRotator.Clamp();
	FVector RotationVector = FVector(ClampedRotator.Pitch, ClampedRotator.Yaw, ClampedRotator.Roll);

	for (int i = 0; i < 3; ++i)
	{
		const float Angle = RotationVector[i];
		if (Angle >= 315.f && Angle <= 45.f)
		{
			RotationVector[i] = 0.f;
		}
		else if (Angle >= 45.f && Angle <= 135.f)
		{
			RotationVector[i] = 90.f;
		}
		else if (Angle >= 135.f && Angle <= 225.f)
		{
			RotationVector[i] = 180.f;
		}
		else if (Angle >= 225.f && Angle <= 315.f)
		{
			RotationVector[i] = 270.f;
		}
	}
	return FRotator(RotationVector.X, RotationVector.Y, RotationVector.Z);
}

float ULimenMath::QuantizeWithHysteresis(const float AngleDegrees, const float StepDegrees, const float HysteresisDegrees, float& LastSnapDegrees)
{
	const float Nearest = FMath::RoundToFloat(AngleDegrees / StepDegrees) * StepDegrees;
	if (FMath::Abs(AngleDegrees - Nearest) <= HysteresisDegrees)   // inside sticky zone
	{
		LastSnapDegrees = Nearest;
		return Nearest;
	}
	// If leaving a snap, keep output continuous until you enter a new sticky zone
	return AngleDegrees;
}

float ULimenMath::SignedAngleAroundAxis(const FVector& From, const FVector& To, const FVector& Axis)
{
	const FVector f = From - FVector::DotProduct(From, Axis) * Axis;
	const FVector t = To   - FVector::DotProduct(To,   Axis) * Axis;
	const float s = FVector::DotProduct(Axis, FVector::CrossProduct(f, t));
	const float c = FVector::DotProduct(f.GetSafeNormal(), t.GetSafeNormal());
	return FMath::RadiansToDegrees(FMath::Atan2(s, c));
}
