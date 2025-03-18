// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LimenStorageItem.h"
#include "LimenArchive.generated.h"

class UTexture;
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
	const FText& GetFullText() const;
	UFUNCTION(BlueprintCallable, Category="Limen|Archives")
	UTexture* GetImage() const;

	UFUNCTION(BlueprintCallable, Category="Limen|Archives")
	TSubclassOf<ULimenArchiveDisplayWidget> GetDisplayWidgetClass() const;

protected:
	UPROPERTY(EditAnywhere)
	TArray<FText> Paragraphs;
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UTexture> ImageTexture;
	UPROPERTY(EditAnywhere)
	TSoftClassPtr<ULimenArchiveDisplayWidget> DisplayWidgetClass;

private:
	UPROPERTY(SaveGame)
	bool bHasBeenRead;

	mutable TOptional<FText> FullArchiveText;
};
