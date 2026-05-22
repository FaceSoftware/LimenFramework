// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actors/LimenGameplayActor.h"
#include "LimenTrigger.generated.h"

class UBoxComponent;

UCLASS(Abstract, NotBlueprintable)
class LIMENTRIGGERS_API ALimenTrigger : public ALimenGameplayActor
{
	GENERATED_BODY()

public:
	ALimenTrigger();
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category="Limen|Triggers")
	UBoxComponent* GetCollisionComponent() const { return CollisionComponent.Get(); }

protected:
	UFUNCTION()
	virtual void ComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                                   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                                   const FHitResult& SweepResult);

	UFUNCTION()
	virtual void ComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                                 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	virtual void ComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                          FVector NormalImpulse, const FHitResult& Hit);

private:
	UPROPERTY(EditDefaultsOnly, BlueprintGetter="GetCollisionComponent", Category="Limen|Components")
	TObjectPtr<UBoxComponent> CollisionComponent;
};
