// Copyright Face Software. All Rights Reserved.


#include "Components/LimenRandomMeshInstance.h"

#include "Subsystems/LimenGlobalRandomStreamSubsystem.h"


ULimenRandomMeshInstance::ULimenRandomMeshInstance()
{
	PrimaryComponentTick.bCanEverTick = false;

	NumberOfInstances = 1;
	MaxScale = 1.f;
	MinScale = 1.f;
}

void ULimenRandomMeshInstance::StartSpawningInstances()
{
	if (StaticMeshes.IsEmpty())
	{
		return;
	}
	
	GlobalRandomStream = ULimenGlobalRandomStreamSubsystem::Get();
	check(GlobalRandomStream)
	
	const uint32 RandomIndex = GlobalRandomStream->GetGlobalRandomStream()->RandRange(0, StaticMeshes.Num() - 1);
	SetStaticMesh(StaticMeshes[RandomIndex].LoadSynchronous());

	InstanceAreaBoundingBox = FBox(InstanceArea.Origin, InstanceArea.Length);
	
	ClearInstances();
	SpawnInstances();
}

TArray<FVector> ULimenRandomMeshInstance::CalculatePoints() const
{
	TArray<FVector> OutPoints;
	OutPoints.SetNum(NumberOfInstances);
	
	for (int i = 0; i < NumberOfInstances; ++i)
	{
		const FVector Point = GlobalRandomStream->GetGlobalRandomStream()->RandPointInBox(InstanceAreaBoundingBox);
		OutPoints[i] = Point;
	}

	return OutPoints;
}

void ULimenRandomMeshInstance::SpawnInstances()
{
	const FTransform RootTransform = GetComponentTransform();
	const TArray<FVector> Points = CalculatePoints();

	TArray<FTransform> Transforms;
	Transforms.Reserve(Points.Num());
	for (const auto& Point : Points)
	{
		FRotator Rotation = FRotator(
			GlobalRandomStream->GetGlobalRandomStream()->FRandRange(0, 360),
			GlobalRandomStream->GetGlobalRandomStream()->FRandRange(0, 360),
			GlobalRandomStream->GetGlobalRandomStream()->FRandRange(0, 360)
			);		

		const float ScaleMultiplier = GlobalRandomStream->GetGlobalRandomStream()->FRandRange(MinScale, MaxScale);
		const FVector Scale = FVector(ScaleMultiplier);

		const FTransform InstanceTransform = FTransform(Rotation, Point, Scale) * RootTransform;
		Transforms.Push(InstanceTransform);
	}

	AddInstances(Transforms, false, true);
}
