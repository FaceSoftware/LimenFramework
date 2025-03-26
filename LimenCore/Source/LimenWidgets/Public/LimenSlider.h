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

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetValue() const;
	UFUNCTION(BlueprintCallable)
	void SetValue(const float NewValue);
	UFUNCTION(BlueprintCallable)
	void SetMaxValue(const float NewMax);
	UFUNCTION(BlueprintCallable)
	void SetMinValue(const float NewMin);
	
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
	TSharedPtr<SPanel> Root;

	float CurrentSliderValue;
	bool bIsDragging;
	FVector2D LastMousePosition;

	EMouseCursor::Type PreviousMouseCursor;
	bool bShouldRevertCursorIcon;

	bool ValidateTextInput(const FString& InputText) const;
	FText GetValueAsText() const;
	void UpdateSliderPositionWithDelta(const FGeometry& MyGeometry, const float DeltaX);
	void UpdateSliderPositionWithCursorPosition(const FGeometry& MyGeometry, const float LocalMouseX);

	void SetValueInternal(const ELimenSliderInput InputType, const float Value, const bool bShouldBroadcast);

	class FDefaultBackgroundBrush final : public FSlateBrush
	{
	public:
		FDefaultBackgroundBrush() : FSlateBrush(ESlateBrushDrawType::Type::Image, TEXT(""),
												FMargin(),
												ESlateBrushTileType::Type::NoTile,
												ESlateBrushImageType::Type::FullColor,
												FVector2D(0.f, 0.f),
												FLinearColor::Transparent, nullptr,
												false)
		{
		}
	};
	
	class FDefaultBorderBrush final : public FSlateBrush
	{
	public:
		FDefaultBorderBrush() : FSlateBrush(ESlateBrushDrawType::Type::RoundedBox, TEXT(""),
											FMargin(),
												ESlateBrushTileType::Type::NoTile,
											ESlateBrushImageType::Type::FullColor,
											FVector2D(0.f, 0.f),
											FLinearColor::Transparent, nullptr,
											false)
		{
			OutlineSettings.Color = FLinearColor::White;
			OutlineSettings.Width = 1.f;
			OutlineSettings.RoundingType = ESlateBrushRoundingType::FixedRadius;
		}
	};
};
