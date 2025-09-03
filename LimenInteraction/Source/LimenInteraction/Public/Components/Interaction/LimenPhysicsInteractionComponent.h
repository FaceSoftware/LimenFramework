// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CollisionQueryParams.h"
#include "LimenInteractionComponent.h"
#include "Engine/HitResult.h"
#include "LimenPhysicsInteractionComponent.generated.h"


class ULimenInteractableAreaComponent;
struct FCollisionQueryParams;
class UPhysicsHandleComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LIMENINTERACTION_API ULimenPhysicsInteractionComponent : public ULimenInteractionComponent
{
	GENERATED_BODY()

public:
	ULimenPhysicsInteractionComponent();

	virtual bool Interact(AController* InController, APawn* InPawn) override;
	virtual void StopInteraction(AController* InController, APawn* InPawn) override;

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
	virtual void Interacted(UActorComponent* Component) override;
	virtual void InteractionStopped(UActorComponent* Component) override;

private:
	TWeakObjectPtr<AController> InteractController;
	TWeakObjectPtr<APawn> InteractPawn;

	TWeakObjectPtr<UPhysicsHandleComponent> PhysicsHandle;
	TWeakObjectPtr<UPrimitiveComponent> HeldComponent;
	TWeakObjectPtr<UPrimitiveComponent> TracedComponent;
	TWeakObjectPtr<ULimenInteractableAreaComponent> InteractionComponent;
	FHitResult OutHit;
	FCollisionQueryParams TraceQueryParams;
	FCollisionResponseParams TraceResponseParams;
	FVector GrabLocation;
	FRotator GrabRotation;
};
