// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "LimenGridSpawner.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LIMENCORE_API ULimenGridSpawner : public USceneComponent
{
	GENERATED_BODY()

public:
	ULimenGridSpawner();
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed(bool bDestroyingHierarchy) override;
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, CallInEditor)
	void CreateGrid();

	virtual bool CanCreateGrid() const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Grid", meta=(ClampMin="3"))
	int32 GridX;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Grid", meta=(ClampMin="3"))
	int32 GridY;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Grid")
	TSubclassOf<AActor> GridNodeActorClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Grid")
	FVector2D GridNodeOffset;

	TArray<UChildActorComponent*> GetNodes() const;
	UChildActorComponent* GetNodeAt(const int32 Index) const;
	UChildActorComponent* GetNodeByCoordinate(const FIntVector2& Coordinates) const;
	AActor* GetNodeActorByCoordinate(const FIntVector2& Coordinates) const;

	virtual void GridCreated(const TArray<UChildActorComponent*>& InGridNode);

private:
	TArray<TWeakObjectPtr<UChildActorComponent>> GridNodes;

	void CleanupGrid();
};
