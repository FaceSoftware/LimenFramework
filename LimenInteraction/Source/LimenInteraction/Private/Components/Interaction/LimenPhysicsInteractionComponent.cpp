// Copyright FaceSoftware. All rights reserved.


#include "Components/Interaction/LimenPhysicsInteractionComponent.h"

#include "DrawDebugHelpers.h"
#include "Components/PrimitiveComponent.h"
#include "Components/Interactable/LimenInteractableAreaComponent.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"


ULimenPhysicsInteractionComponent::ULimenPhysicsInteractionComponent()
{
	TraceChannel = ECC_Visibility;
	HandleLinearStiffness = 4000.f;
	HandleLinearDamping = 200.f;
	HandleAngularStiffness = 1500.f;
	HandleAngularDamping = 100.f;
	ThrowImpulseStrength = 1200.f;
	InterpolationSpeed = 50.f;
	GrabLocation = FVector::ZeroVector;
	GrabRotation = FRotator::ZeroRotator;
}

void ULimenPhysicsInteractionComponent::SetupInteraction()
{
	Super::SetupInteraction();

	PhysicsHandle = NewObject<UPhysicsHandleComponent>(GetOwner());
	PhysicsHandle->InterpolationSpeed = InterpolationSpeed;
	PhysicsHandle->LinearDamping = HandleLinearDamping;
	PhysicsHandle->LinearStiffness = HandleLinearStiffness;
	PhysicsHandle->AngularDamping = HandleAngularDamping;
	PhysicsHandle->AngularStiffness = HandleAngularStiffness;
	PhysicsHandle->bSoftAngularConstraint = true;
	PhysicsHandle->bSoftLinearConstraint = true;
	PhysicsHandle->RegisterComponent();

	TraceQueryParams.AddIgnoredSourceObject(GetOwner());
	TraceQueryParams.bTraceComplex = false;
	TraceQueryParams.bIgnoreBlocks = false;
	TraceQueryParams.bIgnoreTouches = false;

	TraceResponseParams = FCollisionResponseParams(ECR_Block);
}

void ULimenPhysicsInteractionComponent::UpdateInteraction(const float DeltaTime)
{
	Super::UpdateInteraction(DeltaTime);

	if (!GetOwner()) return;

	FVector Location;
	FRotator Rotation;
	GetOwner()->GetActorEyesViewPoint(Location, Rotation);

	const FVector TraceStart = Location;
	const FVector TraceEnd = Location + Rotation.Vector() * GetInteractionRange();

	GetWorld()->LineTraceSingleByChannel(OutHit, TraceStart, TraceEnd, TraceChannel, TraceQueryParams);
	
	if (OutHit.bBlockingHit)
	{
		SetCurrentInteractableInterface(OutHit.GetComponent());
	}
	else
	{
		SetCurrentInteractableInterface(nullptr);
	}

	if (GetInteractionPawn() && GetCurrentInteractableInterface())
	{
		FVector EyesLocation;
		FRotator EyesRotation;
		GetInteractionPawn()->GetActorEyesViewPoint(EyesLocation, EyesRotation);

		const FVector TargetLocation = EyesLocation + EyesRotation.Vector() * GrabLocation.Length();
		// const FRotator TargetRotation = EyesRotation;
		if (PhysicsHandle->GetGrabbedComponent())
		{
			PhysicsHandle->SetTargetLocation(TargetLocation);
		}
	}

	if (DebugMode())
	{
		DrawDebugSphere(GetWorld(), OutHit.bBlockingHit ? OutHit.ImpactPoint : TraceEnd,
						16.f, 4, FColor::Blue, false, DeltaTime,
						0.f, 0.f);
	}
}

void ULimenPhysicsInteractionComponent::Interacted(UActorComponent* Component)
{
	if (!GetCurrentInteractableInterface().GetObject()) return;
	if (!GetCurrentInteractableInterface()->AllowPhysicsInteraction()) return;
	if (!GetCurrentInteractableInterface()->GetPrimitiveComponent()->IsSimulatingPhysics()) return;
	if (!GetInteractionPawn()) { return; }
	if (!GetInteractionController()) { return; }

	UPrimitiveComponent* InteractionComponent = GetCurrentInteractableInterface()->GetPrimitiveComponent();
	if (!InteractionComponent) return;

	FVector EyesLocation;
	FRotator EyesRotation;
	GetInteractionPawn()->GetActorEyesViewPoint(EyesLocation, EyesRotation);
	const FTransform EyesTransform = FTransform(EyesRotation, EyesLocation);

	{
		const FQuat RelQuat = EyesTransform.InverseTransformRotation(InteractionComponent->GetComponentQuat());
		GrabRotation = RelQuat.Rotator();
	}
	{

		const FVector RelLocation = EyesTransform.InverseTransformPosition(InteractionComponent->GetComponentLocation());
		GrabLocation = RelLocation;
	}

	TraceQueryParams.AddIgnoredSourceObject(InteractionComponent);

	const FVector TargetLocation = EyesLocation + EyesRotation.Vector() * GrabLocation.Length();
	// const FRotator TargetRotation = FRotator::ZeroRotator;
	PhysicsHandle->GrabComponentAtLocation(InteractionComponent, NAME_None, TargetLocation);
	
	Super::Interacted(Component);
}

void ULimenPhysicsInteractionComponent::InteractionStopped(UActorComponent* Component)
{
	Super::InteractionStopped(Component);

	TraceQueryParams.ClearIgnoredSourceObjects();
	TraceQueryParams.AddIgnoredSourceObject(GetOwner());
	SetCurrentInteractableInterface(nullptr);

	GrabLocation = FVector::ZeroVector;
	GrabRotation = FRotator::ZeroRotator;

	PhysicsHandle->ReleaseComponent();
}
