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


UENUM(BlueprintType)
enum class ELimenSliderInput : uint8
{
	Undefined = 0,
	Typed,
	MouseDrag,
};

UENUM(BlueprintType)
enum class ELimenSliderInputMethod : uint8
{
	Drag,
	MousePosition,
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FLimenSliderEvent, ELimenSliderInput, InputType, float, NewValue);

/**
 * 
 */
UCLASS()
class LIMENWIDGETS_API ULimenSlider : public UWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FLimenSliderEvent OnNewValueSet;
	
	ULimenSlider();

	UFUNCTION(BlueprintCallable)
	float GetValue() const;
	UFUNCTION(BlueprintCallable)
	void SetValue(const float NewValue);
	UFUNCTION(BlueprintCallable)
	void SetMaxValue(const float NewMax);
	UFUNCTION(BlueprintCallable)
	void SetMinValue(const float NewMin);
	UFUNCTION(BlueprintCallable)
	void SetDecimalDigitsCount(const int32 NewDecimalDigits);
	UFUNCTION(BlueprintCallable)
	int32 GetDecimalDigitsCount() const;

protected:
	UPROPERTY(EditAnywhere)
	ELimenSliderInputMethod SliderInputMethod;
	UPROPERTY(EditAnywhere)
	float InitialSliderValue;
	UPROPERTY(EditAnywhere)
	int32 DecimalDigits;
	UPROPERTY(EditAnywhere)
	float SliderMinValue;
	UPROPERTY(EditAnywhere)
	float SliderMaxValue;
	
	UPROPERTY(EditAnywhere)
	FSlateBrush BackgroundBrush;
	UPROPERTY(EditAnywhere)
	FSlateBrush BackgroundHoveredBrush;

	UPROPERTY(EditAnywhere)
	FSlateBrush BorderBrush;
	UPROPERTY(EditAnywhere)
	FMargin BorderPadding;
	
	UPROPERTY(EditAnywhere)
	FSlateBrush SliderBrush;
	UPROPERTY(EditAnywhere)
	FMargin SliderPadding;

	UPROPERTY(EditAnywhere)
	bool bUseValueText;
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

	virtual void OnHover(const FGeometry& InGeometry, const FPointerEvent& Event);
	virtual void OnUnHover(const FPointerEvent& Event);
	virtual FReply OnPressed(const FGeometry& InGeometry, const FPointerEvent& Event);
	virtual FReply OnReleased(const FGeometry& InGeometry, const FPointerEvent& Event);
	virtual FReply OnMoved(const FGeometry& InGeometry, const FPointerEvent& Event);

	virtual void TextValueChanged(const FText& InText);
	virtual void TextValueCommited(const FText& InText, const ETextCommit::Type CommitType);

private:
	TSharedPtr<SMouseDetector> InputDetector;
	TSharedPtr<SImage> SliderImage;
	TSharedPtr<SEditableText> NumberText;
	TSharedPtr<SImage> SliderBackground;
	TSharedPtr<SPanel> Root;

	float CurrentSliderValue;
	bool bIsDragging;
	bool bIsHovering;
	FVector2D LastMousePosition;

	EMouseCursor::Type PreviousMouseCursor;
	bool bShouldRevertCursorIcon;

	FDelegateHandle WorldTickDelegate;
	bool bBroadcastValueSetDelegate;
	ELimenSliderInput LastInputType;
	

	bool ValidateTextInput(const FString& InputText) const;
	FText GetValueAsText() const;
	void UpdateSliderPositionWithDelta(const FGeometry& MyGeometry, const float DeltaX);
	void UpdateSliderPositionWithCursorPosition(const FGeometry& MyGeometry, const float LocalMouseX);
	void SetValueInternal(const ELimenSliderInput InputType, const float Value, const bool bShouldBroadcast);
	void WorldTick(UWorld* WorldPtr, const ELevelTick LevelTick, const float DeltaTime);
};
