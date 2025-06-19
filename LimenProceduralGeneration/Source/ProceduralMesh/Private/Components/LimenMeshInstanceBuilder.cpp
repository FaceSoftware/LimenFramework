// Copyright Face Software. All Rights Reserved.


#include "Components/LimenMeshInstanceBuilder.h"

#include "Engine/StaticMesh.h"


ULimenMeshInstanceBuilder::ULimenMeshInstanceBuilder()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void ULimenMeshInstanceBuilder::BeginPlay()
{
	// StartSpawningInstances();
	Super::BeginPlay();
}

void ULimenMeshInstanceBuilder::StartSpawningInstances()
{
	if (!GetStaticMesh())
	{
		return;
	}
	
	InstanceAreaBoundingBox = FBox(GenerationParams.Origin, GenerationParams.Length);
	SpawnInstances();
}

TArray<FVector> ULimenMeshInstanceBuilder::CalculatePoints() const
{	
	const FBox MeshBoundingBox = GetStaticMesh()->GetBoundingBox();
	check(MeshBoundingBox.IsValid)

	FVector SpawnAreaLengthVector;
	SpawnAreaLengthVector.X = InstanceAreaBoundingBox.Max[0] - InstanceAreaBoundingBox.Min[0];
	SpawnAreaLengthVector.Y = InstanceAreaBoundingBox.Max[1] - InstanceAreaBoundingBox.Min[1];
	SpawnAreaLengthVector.Z = InstanceAreaBoundingBox.Max[2] - InstanceAreaBoundingBox.Min[2];
	
	FVector MeshLengthVector;
	MeshLengthVector.X = MeshBoundingBox.Max[0] - MeshBoundingBox.Min[0] + GenerationParams.GetMaxOffset().X;
	MeshLengthVector.Y = MeshBoundingBox.Max[1] - MeshBoundingBox.Min[1] + GenerationParams.GetMaxOffset().Y;
	MeshLengthVector.Z = MeshBoundingBox.Max[2] - MeshBoundingBox.Min[2] + GenerationParams.GetMaxOffset().Z;

	UE::Math::TIntVector3<int32> NumberOfInstances;
	NumberOfInstances.X = FMath::FloorToInt32(FMath::RoundToPositiveInfinity(SpawnAreaLengthVector.X / MeshLengthVector.X)); 
	NumberOfInstances.Y = FMath::FloorToInt32(FMath::RoundToPositiveInfinity(SpawnAreaLengthVector.Y / MeshLengthVector.Y));
	NumberOfInstances.Z = FMath::FloorToInt32(FMath::RoundToPositiveInfinity(SpawnAreaLengthVector.Z / MeshLengthVector.Z));

	TArray<FVector> OutPoints;
	for (int x = 0; x < NumberOfInstances.X; x++)
	{
		const float OffsetX = GenerationParams.GetOffset(EAxis::X);
		for (int y = 0; y < NumberOfInstances.Y; ++y)
		{
			const float OffsetY = GenerationParams.GetOffset(EAxis::Y);
			for (int z = 0; z < NumberOfInstances.Z; ++z)
			{
				const float OffsetZ = GenerationParams.GetOffset(EAxis::Z);

				if (x == 0 || x == NumberOfInstances.X - 1 ||
					y == 0 || y == NumberOfInstances.Y - 1 ||
					z == 0 || z == NumberOfInstances.Z - 1 )
				{
					FVector TempPoint;
					TempPoint.X = x * MeshLengthVector.X + OffsetX;
					TempPoint.Y = y * MeshLengthVector.Y + OffsetY;
					TempPoint.Z = z * MeshLengthVector.Z + OffsetZ;
					OutPoints.Push(TempPoint);
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
	for (const FVector& Point : Points)
	{
		const FTransform InstanceTransform = FTransform(FRotator::ZeroRotator, Point, FVector::OneVector) * GetComponentTransform();
		Transforms.Push(InstanceTransform);
	}

	for (int i = 0; i < GetNumInstances(); ++i)
	{
		RemoveInstance(i);
	}

	InstanceIndices.Empty(Transforms.Num());
	InstanceIndices = AddInstances(Transforms, true, true);
}
