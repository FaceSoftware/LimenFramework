// Fill out your copyright notice in the Description page of Project Settings.


#include "DataAssets/LimenGridItemDatabase.h"

#include "Items/LimenItemBase.h"


bool FGridItemDefinition::operator==(const FGridItemDefinition& Other) const
{
	return Class == Other.Class;
}

bool FGridItemDefinition::operator==(const ALimenItemBase* Item) const
{
	if (Item == nullptr)
	{
		return Class == nullptr;
	}
		
	return Class == Item->GetClass();
}

bool FGridItemDefinition::operator==(const TSubclassOf<ALimenItemBase>& InClass) const
{
	return Class == InClass;
}

const FGridItemDefinition* ULimenGridItemDatabase::GetItemDefinition(const ALimenItemBase* InItem) const
{
	return ItemDefinitions.FindByKey(InItem);
}

const FGridItemDefinition& ULimenGridItemDatabase::GetItemDefinitionChecked(const ALimenItemBase* InItem) const
{
	auto* Element = ItemDefinitions.FindByKey(InItem);
	check(Element)
	return *Element;
}
