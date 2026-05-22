// Copyright FaceSoftware. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LimenWidget.h"
#include "LimenChoiceWidget.generated.h"


USTRUCT(BlueprintType)
struct FGameplayChoiceData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FName Id = NAME_None;
	UPROPERTY(BlueprintReadWrite)
	FText DisplayName = FText();
};

USTRUCT(BlueprintType)
struct FGameplayChoiceScreenData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	TArray<FGameplayChoiceData> Choices;

	UPROPERTY(BlueprintReadWrite)
	FText ChoicePrompt = FText();
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FChoiceDelegate, const FGameplayChoiceData&, ChoiceData);

/**
 * 
 */
UCLASS()
class LIMENWIDGETS_API ULimenChoiceWidget : public ULimenWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FChoiceDelegate OnChoiceSelected;

	UFUNCTION(BlueprintCallable)
	void SetChoicesData(const FGameplayChoiceScreenData& ChoiceData);

protected:
	UFUNCTION(BlueprintCallable)
	void MakeChoice(const FGameplayChoiceData& ChoiceData);

	UFUNCTION(BlueprintImplementableEvent)
	void ChoiceDataReceived(const FGameplayChoiceScreenData& ChoiceData);

private:


};
