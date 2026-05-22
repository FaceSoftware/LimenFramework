// Copyright FaceSoftware. All rights reserved.


#include "Components/LimenWidgetInteractionComponent.h"


ULimenWidgetInteractionComponent::ULimenWidgetInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	bAutoActivate = false;
}

void ULimenWidgetInteractionComponent::Activate(bool bReset)
{
	Super::Activate(bReset);
}

void ULimenWidgetInteractionComponent::Deactivate()
{
	Super::Deactivate();
}

void ULimenWidgetInteractionComponent::PressPointerKey(const FKey Key)
{
	Super::PressPointerKey(Key);
}
