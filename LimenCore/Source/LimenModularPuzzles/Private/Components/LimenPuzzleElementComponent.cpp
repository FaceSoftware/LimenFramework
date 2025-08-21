// Copyright FaceSoftware. All Rights Reserved.


#include "Components/LimenPuzzleElementComponent.h"


ULimenPuzzleElementComponent::ULimenPuzzleElementComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	bAutoActivate = false;

	ElementId = NAME_None;
}

void ULimenPuzzleElementComponent::BeginPlay()
{
	Super::BeginPlay();

	check(!bAutoActivate)
}

const FName& ULimenPuzzleElementComponent::GetElementId() const
{
	return ElementId;
}

void ULimenPuzzleElementComponent::SetElementId(const FName& NewId)
{
	ElementId = NewId;
}
