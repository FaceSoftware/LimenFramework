// Copyright Face Software. All Rights Reserved.


#include "Components/Interaction/LimenProximityInteractionComponent.h"

#include "EngineUtils.h"
#include "Components/SphereComponent.h"


const TArray<UPrimitiveComponent*>& ULimenProximityInteractionComponent::GetAllInteractablesInRange() const
{
	return ComponentsInSphere;
}

const TArray<AActor*>& ULimenProximityInteractionComponent::GetAllInteractableActorsInRange() const
{
	return ActorsInSphere;
}

void ULimenProximityInteractionComponent::RestartInteraction()
{
	SetupInteraction();
}

void ULimenProximityInteractionComponent::SetupInteraction()
{
	Super::SetupInteraction();

	if (InteractionSphere)
	{
		InteractionSphere->DestroyComponent();
		InteractionSphere = nullptr;
	}
	
	InteractionSphere = NewObject<USphereComponent>(this);
	check(InteractionSphere)
	
	InteractionSphere->SetupAttachment(GetOwner()->GetRootComponent());
	InteractionSphere->SetRelativeLocation(FVector::Zero());
	InteractionSphere->SetSphereRadius(GetInteractionRange());
	InteractionSphere->SetGenerateOverlapEvents(true);
	InteractionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractionSphere->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	
	InteractionSphere->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::OnInteractionSphereBeginOverlap);
	InteractionSphere->OnComponentEndOverlap.AddUniqueDynamic(this, &ThisClass::OnInteractionSphereEndOverlap);

	InteractionSphere->RegisterComponent();

	// Check for actors already inside the interaction sphere
	CheckActorsInInsideRadius();
}

void ULimenProximityInteractionComponent::UpdateInteraction()
{
	Super::UpdateInteraction();

	FVector Start, End;
	if (!GetOwnerLineOfSightStartAndEndVectors(Start, End))
	{
		return;
	}

	UPrimitiveComponent* ClosestComponent = nullptr;
	float ClosestDistance = TNumericLimits<double>::Max();
	for (auto* Component : ComponentsInSphere)
	{
		const float CurrentDistance = FVector::DistSquared(End, Component->GetComponentLocation());
		if (CurrentDistance > ClosestDistance)
		{
			continue;
		}

		ClosestComponent = Component;
		ClosestDistance = CurrentDistance;
	}

	CurrentInteractableInterface = ClosestComponent;

	if (DebugMode())
	{
		DrawDebugSphere(GetWorld(), GetOwner()->GetRootComponent()->GetComponentLocation(), GetInteractionRange(), 16, FColor::Green);
	}
}

void ULimenProximityInteractionComponent::OnInteractionSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (!IsInteractableComponent(OtherComp))
	{
		return;
	}

	const auto PrimitiveComponent = Cast<UPrimitiveComponent>(OtherComp);
	check(PrimitiveComponent)

	if (!IsInsideSphere(PrimitiveComponent))
	{
		ComponentsInSphere.AddUnique(PrimitiveComponent);
		ActorsInSphere.AddUnique(PrimitiveComponent->GetOwner());
	}
}

void ULimenProximityInteractionComponent::OnInteractionSphereEndOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!IsInteractableComponent(OtherComp))
	{
		return;
	}

	const auto PrimitiveComponent = Cast<UPrimitiveComponent>(OtherComp);
	check(PrimitiveComponent)

	if (!IsInsideSphere(PrimitiveComponent))
	{
		ComponentsInSphere.Remove(PrimitiveComponent);
		ActorsInSphere.Remove(PrimitiveComponent->GetOwner());
	}
}

bool ULimenProximityInteractionComponent::IsInsideSphere(const UPrimitiveComponent* InteractableComponent) const
{
	const float Distance = FVector::DistSquared(InteractableComponent->GetComponentLocation(), GetOwner()->GetActorLocation());
	return Distance <= FMath::Square(GetInteractionRange());
}

void ULimenProximityInteractionComponent::CheckActorsInInsideRadius()
{
	ActorsInSphere.Empty();
	ComponentsInSphere.Empty();
	
	const float SquaredInteractionRange = FMath::Square(GetInteractionRange());
	float CurrentActorDistance = 0.f;
	for (TActorIterator<AActor> It(GetWorld()); It; ++It)
	{
		CurrentActorDistance = FVector::DistSquared(It->GetActorLocation(), GetOwner()->GetActorLocation());
		if (CurrentActorDistance > SquaredInteractionRange)
		{
			continue;
		}

		TArray<UPrimitiveComponent*> PrimitiveComponents;
		It->GetComponents<UPrimitiveComponent>(PrimitiveComponents);
		for (auto* Component : PrimitiveComponents)
		{
			if (IsInteractableComponent(Component))
			{
				ComponentsInSphere.Push(Component);
				ActorsInSphere.Push(Component->GetOwner());
				break;
			}	
		}
	}
}
