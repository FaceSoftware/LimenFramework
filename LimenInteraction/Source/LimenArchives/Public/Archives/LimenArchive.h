// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LimenStorageItem.h"
#include "LimenArchive.generated.h"

class ULimenArchiveComponent;
class ULimenArchiveDisplayWidget;
/**
 * 
 */
UCLASS(Abstract)
class LIMENARCHIVES_API ULimenArchive : public ULimenStorageItem
{
	GENERATED_BODY()

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FArchiveDelegate);

	UPROPERTY(BlueprintAssignable)
	FArchiveDelegate OnArchiveRead;
	
	ULimenArchive();
	
	UFUNCTION(BlueprintCallable, Category="Limen|Archives")
	bool HasBeenRead() const;
	UFUNCTION(BlueprintCallable, Category="Limen|Archives")
	void SetHasBeenRead();
	UFUNCTION(BlueprintCallable, Category="Limen|Archives")
	const TArray<FText>& GetParagraphs() const;
	UFUNCTION(BlueprintCallable, Category="Limen|Archives")
	TSubclassOf<ULimenArchiveDisplayWidget> GetDisplayWidgetClass() const;

protected:

private:
	UPROPERTY(SaveGame)
	bool bHasBeenRead;
	
	UPROPERTY(EditAnywhere)
	TArray<FText> Paragraphs;
	UPROPERTY(EditAnywhere)
	TSoftClassPtr<ULimenArchiveDisplayWidget> DisplayWidgetClass;
};
