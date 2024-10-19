// Copyright Face Software. All Rights Reserved.


#include "LimenStorageItem.h"

const FName& ULimenStorageItem::GetDevelopmentName() const
{
	return DevelopmentName;
}

const FText& ULimenStorageItem::GetCategory() const
{
	return Category;
}

const FText& ULimenStorageItem::GetDisplayName() const
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

void ULimenStorageItem::DataSaved()
{
}

void ULimenStorageItem::DataLoaded()
{
}
