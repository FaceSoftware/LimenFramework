// Copyright Face Software. All Rights Reserved.


#include "LimenStorageItem.h"

FName ULimenStorageItem::GetDevelopmentName() const
{
	return DevelopmentName;
}

FText ULimenStorageItem::GetCategory() const
{
	return Category;
}

FText ULimenStorageItem::GetDisplayName() const
{
	return DisplayName;
}

bool ULimenStorageItem::ShouldSaveData() const
{
	return true;
}

bool ULimenStorageItem::ShouldLoadData() const
{
	return true;
}

void ULimenStorageItem::PreDataSaved()
{
}

void ULimenStorageItem::PostDataSaved()
{
}

void ULimenStorageItem::PreDataLoaded()
{
}

void ULimenStorageItem::PostDataLoaded()
{
}

FName ULimenStorageItem::GetUniqueDeterministicId() const
{
	return DevelopmentName;
}
