// Copyright Face Software. All Rights Reserved.


#include "Components/Interactable/LimenInteractablePromptComponent.h"


ULimenInteractablePromptComponent::ULimenInteractablePromptComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

const FText& ULimenInteractablePromptComponent::GetPromptText() const
{
	return InteractionPrompt;
}

void ULimenInteractablePromptComponent::SetPromptText(const FText& NewPrompt)
{
	InteractionPrompt = NewPrompt;
	OnPromptChanged.Broadcast(InteractionPrompt);
}
