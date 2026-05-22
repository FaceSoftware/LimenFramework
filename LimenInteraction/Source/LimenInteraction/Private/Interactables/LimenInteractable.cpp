// Copyright Face Software. All Rights Reserved.


#include "Interactables/LimenInteractable.h"

#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Components/Interactable/LimenInteractableComponent.h"


ALimenInteractable::ALimenInteractable(const FObjectInitializer& InObjectInitializer) : Super(InObjectInitializer)
{
	bWasInteracted = false;
	bIsBeingContinuouslyInteracted = false;
}

void ALimenInteractable::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	for (ULimenInteractableComponent* InteractableAreaComponent : GetInteractableComponents())
	{
		InteractableAreaComponent->OnInteract.AddUObject(this, &ThisClass::OnInteract_Internal);
		InteractableAreaComponent->OnInteractionStopped.AddUObject(this, &ThisClass::OnInteractionStopped_Internal);
	}
}

bool ALimenInteractable::HasBeenInteracted() const
{
	return bWasInteracted;
}

TArray<ULimenInteractableComponent*> ALimenInteractable::GetInteractableComponents() const
{
	TArray<ULimenInteractableComponent*> InteractableComponents;
	GetComponents<ULimenInteractableComponent>(InteractableComponents);
	return InteractableComponents;
}

bool ALimenInteractable::ShouldSaveData() const
{
	return true;
}

bool ALimenInteractable::ShouldLoadData() const
{
	return true;
}

void ALimenInteractable::PreDataSaved()
{
}

void ALimenInteractable::PostDataLoaded()
{
}

FName ALimenInteractable::GetUniqueDeterministicId() const
{
	return UniqueName;
}

void ALimenInteractable::SetUniqueName(const FName& NewUniqueName)
{
	UniqueName = NewUniqueName;
}

void ALimenInteractable::PostDataSaved()
{
}

void ALimenInteractable::PreDataLoaded()
{
}

bool ALimenInteractable::IsBeingContinuouslyInteracted() const
{
	return bIsBeingContinuouslyInteracted;
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
	Interact(InController, InPawn);
	OnInteract.Broadcast(InController, InPawn);
}

void ALimenInteractable::OnInteractionStopped_Internal(AController* InController, APawn* InPawn)
{
	InteractionStopped(InController, InPawn);
	OnInteractionStopped.Broadcast(InController, InPawn);
}
