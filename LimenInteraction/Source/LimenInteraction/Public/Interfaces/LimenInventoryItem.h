// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "LimenInventoryItem.generated.h"


class ULimenItemAction;

// This class does not need to be modified.
UINTERFACE()
class ULimenInventoryItem : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class LIMENINTERACTION_API ILimenInventoryItem
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual UTexture* GetItemImage() const = 0;
	virtual const FText& GetDisplayName() const = 0;
	virtual const FText& GetDescription() const = 0;
	virtual TArray<ULimenItemAction*> GetItemActions() const = 0;
	virtual int32 GetItemQuantity() const = 0;
	virtual AActor* GetActor() = 0;
	
	template<typename T>
	T* GetActor()
	{
		static_assert(TIsDerivedFrom<T, AActor>::Value);
		return Cast<T>(GetActor());
	}
};
