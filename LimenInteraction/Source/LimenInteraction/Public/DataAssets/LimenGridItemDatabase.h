// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LimenGridItemDatabase.generated.h"


class ALimenItemBase;

USTRUCT()
struct LIMENINTERACTION_API FGridItemDefinition
{
	GENERATED_BODY()
	
	FGridItemDefinition()
	{
		Class = nullptr;
		Size = FIntVector2(1);
		Icon.TintColor = FSlateColor(FLinearColor::Transparent);
		bCanStack = false;
		StackLimit = 2;
	}
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<ALimenItemBase> Class = nullptr;
	
	UPROPERTY(EditAnywhere)
	FIntVector2 Size = FIntVector2(1);
	
	UPROPERTY(EditAnywhere)
	FSlateBrush Icon;

	UPROPERTY(EditAnywhere)
	bool bCanStack = false;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "bCanStack", ClampMin = 2))
	int32 StackLimit = 2;
	
	bool operator==(const FGridItemDefinition& Other) const;
	bool operator==(const ALimenItemBase* Item) const;
	bool operator==(const TSubclassOf<ALimenItemBase>& InClass) const;
};

/**
 * 
 */
UCLASS()
class LIMENINTERACTION_API ULimenGridItemDatabase : public UDataAsset
{
	GENERATED_BODY()
	
public:
	const FGridItemDefinition* GetItemDefinition(const ALimenItemBase* InItem) const;
	const FGridItemDefinition& GetItemDefinitionChecked(const ALimenItemBase* InItem) const;

protected:
	UPROPERTY(EditAnywhere)
	TArray<FGridItemDefinition> ItemDefinitions;
};
