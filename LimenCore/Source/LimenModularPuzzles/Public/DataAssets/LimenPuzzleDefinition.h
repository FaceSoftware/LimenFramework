// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LimenPuzzleDefinition.generated.h"


struct FPuzzleConditionNode;

UCLASS()
class LIMENMODULARPUZZLES_API UPuzzleDefinition : public UDataAsset
{
	GENERATED_BODY()

public:
	UPuzzleDefinition()
	{
		OutputId = NAME_None;
	}

	UPROPERTY(EditAnywhere)
	TArray<FName> PuzzleElementIds;
	UPROPERTY(EditAnywhere)
	TArray<FPuzzleConditionNode> Graph;
	UPROPERTY(EditAnywhere)
	FName OutputId;
};
