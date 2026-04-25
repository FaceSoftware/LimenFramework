// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/LimenSaveObjectInterface.h"
#include "UObject/Object.h"
#include "LimenStorageItem.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable, BlueprintType)
class LIMENSTORAGE_API ULimenStorageItem : public UObject, public ILimenSaveObjectInterface
{
	GENERATED_BODY()

public:	
	UPROPERTY(EditAnywhere)
	FName DevelopmentName;
	UPROPERTY(EditAnywhere)
	FText Category;
	UPROPERTY(EditAnywhere)
	FText DisplayName;

	UFUNCTION(BlueprintCallable, Category="Limen|Archives")
	FName GetDevelopmentName() const;
	UFUNCTION(BlueprintCallable, Category="Limen|Archives")
	FText GetCategory() const;
	UFUNCTION(BlueprintCallable, Category="Limen|Archives")
	FText GetDisplayName() const;

	virtual bool ShouldSaveData() const override;
	virtual bool ShouldLoadData() const override;
	virtual void PreDataSaved() override;
	virtual void PostDataSaved() override;
	virtual void PreDataLoaded() override;
	virtual void PostDataLoaded() override;
	virtual FName GetUniqueDeterministicId() const override;

protected:
};
