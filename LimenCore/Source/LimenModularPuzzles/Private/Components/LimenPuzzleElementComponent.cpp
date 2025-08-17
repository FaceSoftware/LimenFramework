// Fill out your copyright notice in the Description page of Project Settings.


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
