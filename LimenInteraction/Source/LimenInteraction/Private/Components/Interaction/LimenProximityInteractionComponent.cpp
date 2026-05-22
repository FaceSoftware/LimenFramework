// Copyright Face Software. All Rights Reserved.


#include "Components/Interaction/LimenProximityInteractionComponent.h"

#include "EngineUtils.h"
#include "Components/SphereComponent.h"
#include "Components/Interactable/LimenInteractableComponent.h"


const TArray<ULimenInteractableComponent*>& ULimenProximityInteractionComponent::GetAllInteractablesInRange() const
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

void ULimenProximityInteractionComponent::UpdateInteraction(const float DeltaTime)
{
	FVector Start, End;
	if (!GetOwnerViewPoint(Start, End))
	{
		return;
	}

	ULimenInteractableComponent* ClosestComponent = nullptr;
	float ClosestDistance = TNumericLimits<double>::Max();
	for (auto* Component : ComponentsInSphere)
	{
		auto* TempComponent = Cast<ULimenInteractableComponent>(Component);
		if (TempComponent == nullptr) { continue; }
		
		const float CurrentDistance = FVector::DistSquared(End, TempComponent->GetComponentLocation());
		if (CurrentDistance > ClosestDistance)
		{
			continue;
		}

		ClosestComponent = TempComponent;
		ClosestDistance = CurrentDistance;
	}

	SetCurrentInteractable(ClosestComponent);

	if (DebugMode())
	{
		DrawDebugSphere(GetWorld(), GetOwner()->GetRootComponent()->GetComponentLocation(),
						GetInteractionRange(), 16, FColor::Green, false, DeltaTime);
	}
}

void ULimenProximityInteractionComponent::OnInteractionSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	const FHitResult& SweepResult)
{
	auto* Interactable = Cast<ULimenInteractableComponent>(OtherComp);
	if (!Interactable) { return; }

	if (!IsInsideSphere(Interactable))
	{
		ComponentsInSphere.AddUnique(Interactable);
		ActorsInSphere.AddUnique(Interactable->GetOwner());
	}
}

void ULimenProximityInteractionComponent::OnInteractionSphereEndOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	auto* Interactable = Cast<ULimenInteractableComponent>(OtherComp);
	if (!Interactable) { return; }

	if (!IsInsideSphere(Interactable))
	{
		ComponentsInSphere.Remove(Interactable);
		ActorsInSphere.Remove(Interactable->GetOwner());
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
			if (auto* Interactable = Cast<ULimenInteractableComponent>(Component))
			{
				ComponentsInSphere.Push(Interactable);
				ActorsInSphere.Push(Interactable->GetOwner());
				break;
			}	
		}
	}
}
