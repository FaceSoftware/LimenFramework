// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LimenInteractionComponent.h"
#include "LimenProximityInteractionComponent.generated.h"


class USphereComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LIMENINTERACTION_API ULimenProximityInteractionComponent : public ULimenInteractionComponent
{
	GENERATED_BODY()

public:
	const TArray<ULimenInteractableComponent*>& GetAllInteractablesInRange() const;
	const TArray<AActor*>& GetAllInteractableActorsInRange() const;
	
	virtual void RestartInteraction() override;

protected:
	virtual void SetupInteraction() override;
	virtual void UpdateInteraction(const float DeltaTime) override;

	UFUNCTION()
	virtual void OnInteractionSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void OnInteractionSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	bool IsInsideSphere(const UPrimitiveComponent* InteractableComponent) const;

	void CheckActorsInInsideRadius();
	
private:
	UPROPERTY()
	TObjectPtr<USphereComponent> InteractionSphere;
	UPROPERTY()
	TArray<ULimenInteractableComponent*> ComponentsInSphere;
	UPROPERTY()
	TArray<AActor*> ActorsInSphere;
	
};
