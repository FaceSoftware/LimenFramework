// Fill out your copyright notice in the Description page of Project Settings.


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

bool ULimenPhysicsInteractionComponent::Interact(AController* InController, APawn* InPawn)
{
	InteractController = InController;
	InteractPawn = InPawn;

	if (!Super::Interact(InController, InPawn)) return false;
	return true;
}

void ULimenPhysicsInteractionComponent::StopInteraction(AController* InController, APawn* InPawn)
{
	InteractController.Reset();
	InteractPawn.Reset();

	Super::StopInteraction(InController, InPawn);
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
	
	if (OutHit.bBlockingHit && OutHit.GetComponent()->IsSimulatingPhysics())
	{
		TracedComponent = OutHit.GetComponent();
		check(OutHit.GetActor())
		if (auto* Component = OutHit.GetActor()->GetComponentByClass<ULimenInteractableAreaComponent>())
		{
			SetCurrentInteractableInterface(Component);
		}
	}
	else
	{
		TracedComponent.Reset();
	}

	if (InteractPawn.IsValid() && HeldComponent.IsValid())
	{
		FVector EyesLocation;
		FRotator EyesRotation;
		InteractPawn->GetActorEyesViewPoint(EyesLocation, EyesRotation);

		const FVector TargetLocation = EyesLocation + EyesRotation.Vector() * GrabLocation.Length();
		const FRotator TargetRotation = EyesRotation;
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
	Super::Interacted(Component);

	HeldComponent = TracedComponent;
	if (!HeldComponent.IsValid()) return;

	FVector EyesLocation;
	FRotator EyesRotation;
	InteractPawn->GetActorEyesViewPoint(EyesLocation, EyesRotation);
	const FTransform EyesTransform = FTransform(EyesRotation, EyesLocation);

	{
		const FQuat RelQuat = EyesTransform.InverseTransformRotation(HeldComponent->GetComponentQuat());
		GrabRotation = RelQuat.Rotator();
	}
	{

		const FVector RelLocation = EyesTransform.InverseTransformPosition(HeldComponent->GetComponentLocation());
		GrabLocation = RelLocation;
	}

	TraceQueryParams.AddIgnoredSourceObject(HeldComponent.Get());

	const FVector TargetLocation = EyesLocation + EyesRotation.Vector() * GrabLocation.Length();
	const FRotator TargetRotation = FRotator::ZeroRotator;
	PhysicsHandle->GrabComponentAtLocation(HeldComponent.Get(), NAME_None, TargetLocation);
}

void ULimenPhysicsInteractionComponent::InteractionStopped(UActorComponent* Component)
{
	Super::InteractionStopped(Component);

	TraceQueryParams.ClearIgnoredSourceObjects();
	TraceQueryParams.AddIgnoredSourceObject(GetOwner());
	HeldComponent.Reset();
	SetCurrentInteractableInterface(nullptr);

	GrabLocation = FVector::ZeroVector;
	GrabRotation = FRotator::ZeroRotator;

	PhysicsHandle->ReleaseComponent();
}
