// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LimenItemSpawner.generated.h"


USTRUCT(BlueprintType)
struct FItemSpawnerParameters
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	bool bAllowNoSpawn = false;
	
	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<ALimenItemBase>> ItemClasses;

	UPROPERTY(EditAnywhere, meta=(ClampMin=0, ClampMax=100, EditCondition="bAllowNoSpawn"))
	double ChanceToSpawn = 50.0;

	UPROPERTY(EditAnywhere, meta=(ClampMin=1))
	uint16 MaxItemsToSpawn = 1;
};

class ALimenItemBase;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LIMENINTERACTION_API ULimenItemSpawner : public USceneComponent
{
	GENERATED_BODY()

public:
	ULimenItemSpawner();

	UFUNCTION(BlueprintCallable, Category="Limen Item Spawner")
	void SpawnItems();

	UFUNCTION(BlueprintCallable, Category="Limen Item Spawner")
	void SetItemClasses(const TArray<TSubclassOf<ALimenItemBase>>& InItemClasses);

	UFUNCTION(BlueprintCallable, Category="Limen Item Spawner")
	void AddItemToSpawn(const TSubclassOf<ALimenItemBase>& InItem);
	
	UFUNCTION(BlueprintCallable, Category="Limen Item Spawner")
	void SetSpawnerParameters(const FItemSpawnerParameters& InParams);
	
protected:
	UPROPERTY(EditAnywhere)
	FItemSpawnerParameters ItemSpawnerParameters;
	
	TArray<ALimenItemBase*> GetSpawnedItems() const;

private:
	UPROPERTY()
	TArray<ALimenItemBase*> SpawnedItems;
};
