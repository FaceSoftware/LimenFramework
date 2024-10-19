// Copyright Face Software. All Rights Reserved.


#include "Components/LimenMeshInstanceBuilder.h"


ULimenMeshInstanceBuilder::ULimenMeshInstanceBuilder()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void ULimenMeshInstanceBuilder::OnComponentCreated()
{
	Super::OnComponentCreated();
	StartSpawningInstances();
}

void ULimenMeshInstanceBuilder::StartSpawningInstances()
{
	if (!GetStaticMesh())
	{
		return;
	}
	
	InstanceAreaBoundingBox = FBox(InstanceArea.Origin, InstanceArea.Length);
	
	SpawnInstances();
}

TArray<FVector> ULimenMeshInstanceBuilder::CalculatePoints() const
{	
	FBox MeshBoundingBox = GetStaticMesh()->GetBoundingBox();
	check(MeshBoundingBox.IsValid)

	FVector MeshLengthVector;
	MeshLengthVector.X = MeshBoundingBox.Max[0] - MeshBoundingBox.Min[0];
	MeshLengthVector.Y = MeshBoundingBox.Max[1] - MeshBoundingBox.Min[1];
	MeshLengthVector.Z = MeshBoundingBox.Max[2] - MeshBoundingBox.Min[2];

	FVector SpawnAreaLengthVector;
	SpawnAreaLengthVector.X = InstanceAreaBoundingBox.Max[0] - InstanceAreaBoundingBox.Min[0];
	SpawnAreaLengthVector.Y = InstanceAreaBoundingBox.Max[1] - InstanceAreaBoundingBox.Min[1];
	SpawnAreaLengthVector.Z = InstanceAreaBoundingBox.Max[2] - InstanceAreaBoundingBox.Min[2];
	
	const int NumberOfInstancesX = FMath::RoundToPositiveInfinity(SpawnAreaLengthVector.X / MeshLengthVector.X);
	const int NumberOfInstancesY = FMath::RoundToPositiveInfinity(SpawnAreaLengthVector.Y / MeshLengthVector.Y);
	const int NumberOfInstancesZ = FMath::RoundToPositiveInfinity(SpawnAreaLengthVector.Z / MeshLengthVector.Z);

	TArray<FVector> OutPoints;
	OutPoints.Reserve(NumberOfInstancesX * NumberOfInstancesY * NumberOfInstancesZ);

	FVector Temp;
	for (int x = 0; x < NumberOfInstancesX; x++)
	{
		for (int y = 0; y < NumberOfInstancesY; ++y)
		{
			for (int z = 0; z < NumberOfInstancesZ; ++z)
			{
				if (x == 0 || x == NumberOfInstancesX - 1 ||
					y == 0 || y == NumberOfInstancesY - 1 ||
					z == 0 || z == NumberOfInstancesZ - 1)
				{
					Temp.X = x * MeshLengthVector.X;
					Temp.Y = y * MeshLengthVector.Y;
					Temp.Z = z * MeshLengthVector.Z;
					OutPoints.Push(Temp);
				}
			}
		}
	}

	return OutPoints;
}

void ULimenMeshInstanceBuilder::SpawnInstances()
{
	const TArray<FVector> Points = CalculatePoints();

	TArray<FTransform> Transforms;
	Transforms.Reserve(Points.Num());
	for (const auto& Point : Points)
	{
		const FTransform InstanceTransform = FTransform(FRotator::ZeroRotator, Point, FVector::OneVector) * GetComponentTransform();
		Transforms.Push(InstanceTransform);
	}
	
	AddInstances(Transforms, false, true);
}
