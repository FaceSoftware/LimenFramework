// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/World.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Subsystems/LimenGlobalRandomStreamSubsystem.h"
#include "LimenMeshInstanceBuilder.generated.h"

class FThreadedFunction;

USTRUCT()
struct FGenerationParameters
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FVector Origin = FVector::ZeroVector;
	UPROPERTY(EditAnywhere)
	FVector Length = FVector::ZeroVector;
	UPROPERTY(EditAnywhere, meta=(ClampMin="0", EditCondition="!bRandomOffset"))
	FVector MeshOffset = FVector::ZeroVector;
	UPROPERTY(EditAnywhere)
	bool bRandomOffset = false;
	UPROPERTY(EditAnywhere, meta=(ClampMin="0", EditCondition="bRandomOffset"))
	FVector RandomOffsetMax = FVector::ZeroVector;
	UPROPERTY(EditAnywhere, meta=(ClampMin="0", EditCondition="bRandomOffset"))
	FVector RandomOffsetMin = FVector::ZeroVector;

	float GetOffset(EAxis::Type Axis) const
	{
		if (bRandomOffset)
		{
			ULimenGlobalRandomStreamSubsystem* GlobalRandomStream = ULimenGlobalRandomStreamSubsystem::Get();
			switch (Axis)
			{
			case EAxis::X:
				return GlobalRandomStream->RandomFloatRange(RandomOffsetMax.X, RandomOffsetMin.X);

			case EAxis::Y:
				return GlobalRandomStream->RandomFloatRange(RandomOffsetMax.Y, RandomOffsetMin.Y);

			case EAxis::Z:
				return GlobalRandomStream->RandomFloatRange(RandomOffsetMax.Z, RandomOffsetMin.Z);

			default:
				return 0.f;
			}
		}

		switch (Axis)
		{
		case EAxis::X:
			return MeshOffset.X;

		case EAxis::Y:
			return MeshOffset.Y;

		case EAxis::Z:
			return MeshOffset.Z;

		default:
			return 0.f;
		}
	}

	FVector GetMaxOffset() const
	{
		return bRandomOffset ? RandomOffsetMax : MeshOffset;
	}
};

/**
 * 
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROCEDURALMESH_API ULimenMeshInstanceBuilder : public UInstancedStaticMeshComponent
{
	GENERATED_BODY()

public:
	ULimenMeshInstanceBuilder();
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category="Limen|Random Mesh Instance", CallInEditor)
	virtual void StartSpawningInstances();

protected:
	UPROPERTY(EditAnywhere,  Category="Generation Parameters")
	FGenerationParameters GenerationParams;
	
	FBox InstanceAreaBoundingBox;
	
private:
	TArray<int32> InstanceIndices;

	virtual TArray<FVector> CalculatePoints() const;
	virtual void SpawnInstances();
};
