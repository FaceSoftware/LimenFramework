// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LimenMeshInstanceBuilder.h"
#include "LimenRandomMeshInstance.generated.h"

class ULimenGlobalRandomStreamSubsystem;

/**
 * Randomly spawns various instances of a specified static mesh around an area
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROCEDURALMESH_API ULimenRandomMeshInstance : public ULimenMeshInstanceBuilder
{
	GENERATED_BODY()

public:
	ULimenRandomMeshInstance();

	virtual void StartSpawningInstances() override;
	
protected:	
	UPROPERTY(EditAnywhere,  Category="Limen|Params", meta=(ClampMin="0"))
	uint16 NumberOfInstances;
	UPROPERTY(EditAnywhere,  Category="Limen|Params")
	FRotator MaxRotation;
	UPROPERTY(EditAnywhere,  Category="Limen|Params")
	FRotator MinRotation;
	UPROPERTY(EditAnywhere,  Category="Limen|Params", meta=(ClampMin="0"))
	float MaxScale;
	UPROPERTY(EditAnywhere,  Category="Limen|Params", meta=(ClampMin="0"))
	float MinScale;
	UPROPERTY(EditAnywhere,  Category="Limen|Params")
	TArray<TSoftObjectPtr<UStaticMesh>> StaticMeshes;

private:
	virtual TArray<FVector> CalculatePoints() const override;
	virtual void SpawnInstances() override;
};
