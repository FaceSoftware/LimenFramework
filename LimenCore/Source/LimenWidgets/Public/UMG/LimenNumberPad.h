// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Widget.h"
#include "UMG/Buttons/LimenStandardButton.h"
#include "LimenNumberPad.generated.h"


class ULimenStandardButton;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLimenNumberPadCodeSubmited, bool, bIsCodeCorrect);

/**
 * 
 */
UCLASS()
class LIMENWIDGETS_API ULimenNumberPad : public UWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FLimenNumberPadCodeSubmited OnCodeSubmitted;

	UFUNCTION(BlueprintCallable)
	void ClearScreen();

protected:
	UPROPERTY(EditAnywhere, Category="Number Pad")
	FText SubmitButtonText;
	UPROPERTY(EditAnywhere, Category="Number Pad")
	FText ClearButtonText;
	UPROPERTY(EditAnywhere, Category="Number Pad")
	FString Solution;
	UPROPERTY(EditAnywhere, Category="Number Pad")
	int32 MaxDigits;
	UPROPERTY(EditAnywhere, Category="Number Pad")
	FSlateBrush ScreenBrush;
	UPROPERTY(EditAnywhere, Category="Number Pad")
	FButtonStyle ButtonStyle;
	UPROPERTY(EditAnywhere, Category="Number Pad")
	FSlateFontInfo ButtonFont;
	UPROPERTY(EditAnywhere, Category="Number Pad")
	FSlateFontInfo ScreenFont;
	UPROPERTY(EditAnywhere, Category="Number Pad")
	FMargin ButtonPadding;

	virtual TSharedRef<SWidget> RebuildWidget() override;

	UFUNCTION()
	virtual void NumberPressed(ULimenStandardButton* Button);
	UFUNCTION()
	virtual void ClearPressed(ULimenStandardButton* Button);
	UFUNCTION()
	virtual void SubmitPressed(ULimenStandardButton* Button);

private:
	TSharedPtr<STextBlock> Screen;

	TArray<TStrongObjectPtr<ULimenStandardButton>> NumberButtons;
	TStrongObjectPtr<ULimenStandardButton> SubmitButton;
	TStrongObjectPtr<ULimenStandardButton> ClearButton;
};
