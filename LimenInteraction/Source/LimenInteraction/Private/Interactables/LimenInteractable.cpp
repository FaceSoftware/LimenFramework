// Copyright Face Software. All Rights Reserved.


#include "Interactables/LimenInteractable.h"


ALimenInteractable::ALimenInteractable(const FObjectInitializer& InObjectInitializer) : Super(InObjectInitializer)
{
	bWasInteracted = false;
	InteractionCount = 0;
	bIsBeingContinuouslyInteracted = false;
}

void ALimenInteractable::BeginPlay()
{
	Super::BeginPlay();

	for (ILimenInteractableComponent* InteractableAreaComponent : GetInteractableComponents())
	{
		InteractableAreaComponent->GetInteractionDelegate()->AddUObject(this, &ThisClass::OnInteract_Internal);
		InteractableAreaComponent->GetInteractionStoppedDelegate()->AddUObject(this, &ThisClass::OnInteractionStopped_Internal);
	}
}

bool ALimenInteractable::HasBeenInteracted() const
{
	return bWasInteracted;
}

uint32 ALimenInteractable::GetInteractionCount() const
{
	return InteractionCount;
}

TArray<ILimenInteractableComponent*> ALimenInteractable::GetInteractableComponents() const
{
	TArray<UActorComponent*> InteractableComponents = GetComponentsByInterface(ULimenInteractableComponent::StaticClass());

	TArray<ILimenInteractableComponent*> OutComponents;
	OutComponents.Reserve(InteractableComponents.Num());
	
	for (UActorComponent* InteractableComponent : InteractableComponents)
	{
		checkf(InteractableComponent->Implements<ULimenInteractableComponent>(), TEXT("Interactable components must inherit ILimenInteractableComponent interface"));
		OutComponents.Push(CastChecked<ILimenInteractableComponent>(InteractableComponent));
	}
	
	return OutComponents;
}

bool ALimenInteractable::IsBeingContinuouslyInteracted() const
{
	return bIsBeingContinuouslyInteracted;
}

void ALimenInteractable::SimulateInteraction()
{
	Interact(GetWorld()->GetFirstPlayerController(), GetWorld()->GetFirstPlayerController()->GetPawn());
	BP_OnInteract(GetWorld()->GetFirstPlayerController(), GetWorld()->GetFirstPlayerController()->GetPawn());
}

void ALimenInteractable::Interact(AController* InController, APawn* InPawn)
{
	bWasInteracted = true;
	bIsBeingContinuouslyInteracted = true;
}

void ALimenInteractable::InteractionStopped(AController* InController, APawn* InPawn)
{
	bIsBeingContinuouslyInteracted = false;	
}

void ALimenInteractable::OnInteract_Internal(AController* InController, APawn* InPawn)
{
	InteractionCount++;
	Interact(InController, InPawn);
	BP_OnInteract(InController, InPawn);
	OnInteract.Broadcast(InController, InPawn);
}

void ALimenInteractable::OnInteractionStopped_Internal(AController* InController, APawn* InPawn)
{
	InteractionStopped(InController, InPawn);
	BP_OnInteractionStopped(InController, InPawn);
	OnInteractionStopped.Broadcast(InController, InPawn);
}
