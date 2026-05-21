// Copyright FaceSoftware. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CollisionQueryParams.h"
#include "LimenInteractionComponent.h"
#include "Engine/HitResult.h"
#include "LimenPhysicsInteractionComponent.generated.h"


class ULimenInteractableComponent;
struct FCollisionQueryParams;
class UPhysicsHandleComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LIMENINTERACTION_API ULimenPhysicsInteractionComponent : public ULimenInteractionComponent
{
	GENERATED_BODY()

public:
	ULimenPhysicsInteractionComponent();

protected:
	UPROPERTY(EditAnywhere, Category="Interaction")
	TEnumAsByte<ECollisionChannel> TraceChannel;
	UPROPERTY(EditAnywhere, Category="Interaction")
	float HandleLinearStiffness;
	UPROPERTY(EditAnywhere, Category="Interaction")
	float HandleLinearDamping;
	UPROPERTY(EditAnywhere, Category="Interaction")
	float HandleAngularStiffness;
	UPROPERTY(EditAnywhere, Category="Interaction")
	float HandleAngularDamping;
	UPROPERTY(EditAnywhere, Category="Interaction")
	float ThrowImpulseStrength;
	UPROPERTY(EditAnywhere, Category="Interaction")
	float InterpolationSpeed;

	virtual void SetupInteraction() override;
	virtual void UpdateInteraction(const float DeltaTime) override;
	virtual void Interacted(ULimenInteractableComponent* Component) override;
	virtual void InteractionStopped(ULimenInteractableComponent* Component) override;

private:
	TWeakObjectPtr<UPhysicsHandleComponent> PhysicsHandle;
	FHitResult OutHit;
	FCollisionQueryParams TraceQueryParams;
	FCollisionResponseParams TraceResponseParams;
	FVector GrabLocation;
	FRotator GrabRotation;
};
