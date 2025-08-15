// Fill out your copyright notice in the Description page of Project Settings.


#include "LimenGlobalRandomStream/Public/BlueprintLibraries/LimenGlobalRandomStreamUtils.h"

#include "NavigationSystem.h"
#include "Kismet/KismetMathLibrary.h"


void ULimenGlobalRandomStreamUtils::RandomPercentage(const double ChanceToWin, bool& bWon)
{
	if (FMath::IsNearlyZero(ChanceToWin))
	{
		bWon = false;
		return;
	}

	if (FMath::IsNearlyEqual(ChanceToWin, 100.0))
	{
		bWon = true;
		return;
	}
	
	auto* GlobalStream = ULimenGlobalRandomStreamSubsystem::Get();
	check(GlobalStream)
	
	const double RandomValue = GlobalStream->GetGlobalRandomStream()->GetFraction() * 100.f;
	bWon = RandomValue <= ChanceToWin;
}

void ULimenGlobalRandomStreamUtils::GetRandomReachablePointInCircumference(UObject* Caller, const FVector& Origin,
	const float Radius, int32 NumOfPoints, FVector& OutLocation, bool& bSuccess)
{
	bSuccess = false; // Initialize success flag to false
    
	if (!Caller) return;

	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(Caller->GetWorld());
	if (!NavSys) return;

	TArray<FVector> Points;
	// Generate points in the circumference
	for (int32 i = 0; i < NumOfPoints; ++i)
	{
		float Angle = (360.f / NumOfPoints) * i; // Divide the circle into equal parts based on NumOfPoints
		FVector Direction = FVector(UKismetMathLibrary::DegSin(Angle), UKismetMathLibrary::DegCos(Angle), 0.f); // Calculate direction vector based on angle, ignoring Z for height
		FVector Point = Origin + Direction * Radius; // Calculate point on circumference
		Points.Add(Point);
	}

	TArray<FVector> NavigablePoints;
	FNavLocation NavPoint;

	// Check each point for navigability
	for (const FVector& Point : Points)
	{
		if (NavSys->ProjectPointToNavigation(Point, NavPoint, FVector::ZeroVector, nullptr /* Specify your navigation filter here, if needed */))
		{
			NavigablePoints.Add(NavPoint.Location);
		}
	}

	// If we found any navigable points, pick one at random
	if (NavigablePoints.Num() > 0)
	{
		const int32 Index = ULimenGlobalRandomStreamSubsystem::Get()->GenerateRandomNumbers(NavigablePoints.Num() - 1, 0, 1)[0];
		OutLocation = NavigablePoints[Index];
		bSuccess = true;
	}
}
