// Copyright © 2024 FaceSoftware. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/Widget.h"
#include "Framework/Text/TextLayout.h"
#include "LimenSlider.generated.h"


class SEditableText;
class SMouseDetector;
class SPanel;
class STextBlock;
class SImage;
class SButton;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLimenSliderEvent, float, NewValue);

/**
 * 
 */
UCLASS()
class LIMENWIDGETS_API ULimenSlider : public UWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FLimenSliderEvent NewValueSet;
	
	ULimenSlider();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetValue();
	UFUNCTION(BlueprintCallable)
	void SetValue(const float NewValue);
	UFUNCTION(BlueprintCallable)
	void SetMaxValue(const float NewMax);
	UFUNCTION(BlueprintCallable)
	void SetMinValue(const float NewMin);
	
protected:
	UPROPERTY(EditAnywhere)
	int32 DecimalDigits;
	UPROPERTY(EditAnywhere)
	float SliderMinValue;
	UPROPERTY(EditAnywhere)
	float SliderMaxValue;
	
	UPROPERTY(EditAnywhere)
	FSlateBrush Background;

	UPROPERTY(EditAnywhere)
	FSlateBrush BorderBrush;
	UPROPERTY(EditAnywhere)
	FMargin BorderPadding;
	
	UPROPERTY(EditAnywhere)
	FSlateBrush SliderBrush;
	UPROPERTY(EditAnywhere)
	FMargin SliderPadding;
	
	UPROPERTY(EditAnywhere)
	FSlateColor TextColor;
	UPROPERTY(EditAnywhere)
	FSlateColor InvalidTextColor;
	UPROPERTY(EditAnywhere)
	FSlateFontInfo NumberTextFont;
	UPROPERTY(EditAnywhere)
	TEnumAsByte<ETextJustify::Type> NumberTextJustification;
	UPROPERTY(EditAnywhere)
	FMargin NumberTextMargin;
	UPROPERTY(EditAnywhere)
	float NumberBoxWidth;
	
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void ReleaseSlateResources(const bool bReleaseChildren) override;

	virtual FReply OnPressed(const FGeometry& InGeometry, const FPointerEvent& Event);
	virtual FReply OnReleased(const FGeometry& InGeometry, const FPointerEvent& Event);
	virtual FReply OnMoved(const FGeometry& InGeometry, const FPointerEvent& Event);
	virtual void TextValueChanged(const FText& InText);
	virtual void TextValueCommited(const FText& InText, const ETextCommit::Type CommitType);

private:
	TSharedPtr<SMouseDetector> InputDetector;
	TSharedPtr<SImage> SliderImage;
	TSharedPtr<SEditableText> NumberText;
	TSharedPtr<SPanel> Root;

	float CurrentSliderValue;
	bool bIsDragging;
	FVector2D LastMousePosition;

	bool ValidateTextInput(const FString& InputText) const;
	FText GetValueAsText() const;
	void UpdateSliderPosition(const FGeometry& MyGeometry, const float MouseX);
};
