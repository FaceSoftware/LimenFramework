// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actors/LimenGameplayActor.h"
#include "Managers/LimenAtomicSpawnManager.h"
#include "Runtime/Engine/Classes/Components/BoxComponent.h"
#include "LimenAtomicSpawner.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LIMENMANAGEDSPAWNS_API ULimenAtomicSpawner : public UBoxComponent
{
	GENERATED_BODY()

public:
	ULimenAtomicSpawner();

	void SpawnItem(const TSubclassOf<AActor>& Class, const int32 Amount);
	bool IsTagCompatible(const FName& Test) const;
	int32 GetSpawnedItemsCount() const;
	
protected:

private:
	UPROPERTY()
	TArray<AActor*> SpawnedActors;

	FActorSpawnParameters SpawnParams;

	void SnapToFloor(AActor* Item) const;
};

UCLASS()
class ALimenAtomicSpawnerActor : public ALimenGameplayActor
{
	GENERATED_BODY()

public:
	explicit ALimenAtomicSpawnerActor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	UPROPERTY(EditAnywhere)
	TObjectPtr<ULimenAtomicSpawner> AtomicSpawnerComponent;
	
};
