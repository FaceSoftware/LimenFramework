// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "LimenMeshInstanceBuilder.generated.h"

class FThreadedFunction;

USTRUCT()
struct FArea3D
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FVector Origin = FVector(0.f);
	
	UPROPERTY(EditAnywhere)
	FVector Length = FVector(0.f);
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
	UPROPERTY(EditAnywhere,  Category="Limen|Params")
	FArea3D InstanceArea;
	
	FBox InstanceAreaBoundingBox;
	
private:
	TArray<int32> InstanceIndices;

	virtual TArray<FVector> CalculatePoints() const;
	virtual void SpawnInstances();
};
