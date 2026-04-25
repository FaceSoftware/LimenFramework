// Copyright FaceSoftware. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/Widget.h"
#include "LimenTextLabel.generated.h"

/**
 * 
 */
UCLASS()
class LIMENWIDGETS_API ULimenTextLabel : public UWidget
{
	GENERATED_BODY()
	
public:
	ULimenTextLabel();
	
	virtual void SetLabelTitle(const FText& InTitle);
	virtual void SetLabelTitleFont(const FSlateFontInfo& InTitleFont);
	virtual void SetLabelTitlePadding(const FMargin& InPadding);
	virtual void SetLabelTitleTextColor(const FSlateColor& InColor);

	
	virtual void SetLabelValue(const FText& InValue);
	virtual void SetLabelValueFont(const FSlateFontInfo& InValueFont);
	virtual void SetLabelValuePadding(const FMargin& InPadding);
	virtual void SetLabelValueTextColor(const FSlateColor& InColor);
	
protected:
	UPROPERTY(EditAnywhere, Category="Label")
	FText LabelTitle;
	UPROPERTY(EditAnywhere, Category="Label")
	FSlateFontInfo LabelTitleFont;
	UPROPERTY(EditAnywhere, Category="Label")
	FMargin LabelTitlePadding;
	UPROPERTY(EditAnywhere, Category="Label")
	FSlateColor LabelTitleTextColor;

	UPROPERTY(EditAnywhere, Category="Label")
	FText LabelValue;
	UPROPERTY(EditAnywhere, Category="Label")
	FSlateFontInfo LabelValueFont;
	UPROPERTY(EditAnywhere, Category="Label")
	FMargin LabelValuePadding;
	UPROPERTY(EditAnywhere, Category="Label")
	FSlateColor LabelValueTextColor;
	
	virtual TSharedRef<SWidget> RebuildWidget() override;
	
private:
	TSharedPtr<STextBlock> TitleTextBlock;
	TSharedPtr<STextBlock> ValueTextBlock;
};

UCLASS()
class LIMENWIDGETS_API ULimenLabel : public UWidget
{
	GENERATED_BODY()
	
public:
	ULimenLabel();
	
	virtual void SetLabelTitle(const FText& InTitle);
	virtual void SetLabelTitleFont(const FSlateFontInfo& InTitleFont);
	virtual void SetLabelTitlePadding(const FMargin& InPadding);
	
	virtual void SetLabelValue(UWidget* InWidget);
	virtual UWidget* GetLabelValue() const;
	template<typename T>
	T* GetLabelValue() const
	{
		static_assert(TIsDerivedFrom<T, UWidget>::Value, "T must be derived from UWidget");
		return Cast<T>(GetLabelValue());
	}
	
protected:
	UPROPERTY(EditAnywhere, Category="Label")
	FText LabelTitle;
	UPROPERTY(EditAnywhere, Category="Label")
	FSlateFontInfo LabelTitleFont;
	UPROPERTY(EditAnywhere, Category="Label")
	FMargin LabelTitlePadding;

	UPROPERTY(EditAnywhere, Category="Label")
	FMargin LabelValuePadding;

	UPROPERTY(EditAnywhere, Category="Label")
	TObjectPtr<UWidget> LabelValue;
	
	virtual TSharedRef<SWidget> RebuildWidget() override;
	
private:
	TSharedPtr<STextBlock> TitleTextBlock;
};
