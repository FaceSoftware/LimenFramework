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
	UFUNCTION(BlueprintCallable, Category="Limen|Archives")
	const FName& GetDevelopmentName() const;
	UFUNCTION(BlueprintCallable, Category="Limen|Archives")
	const FText& GetCategory() const;
	UFUNCTION(BlueprintCallable, Category="Limen|Archives")
	const FText& GetDisplayName() const;

	virtual bool ShouldSaveData() const override;
	virtual bool ShouldLoadData() const override;
	virtual void DataSaved() override;
	virtual void DataLoaded() override;

protected:
	UPROPERTY(EditAnywhere)
	FName DevelopmentName;
	UPROPERTY(EditAnywhere)
	FText Category;
	UPROPERTY(EditAnywhere)
	FText DisplayName;
};
