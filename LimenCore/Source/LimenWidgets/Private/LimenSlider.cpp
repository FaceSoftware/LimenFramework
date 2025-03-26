// Copyright © 2024 FaceSoftware. All rights reserved.


#include "LimenSlider.h"

#include "Components/HorizontalBox.h"
#include "Framework/Application/SlateApplication.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SOverlay.h"
#include "Widgets/SPanel.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Text/STextBlock.h"
#include "Slate/SMouseDetector.h"
#include "Widgets/Input/SEditableText.h"
#include "Widgets/Layout/SBox.h"


ULimenSlider::ULimenSlider() : Super(), DecimalDigits(1), SliderMinValue(0), SliderMaxValue(1),
							   NumberTextJustification(ETextJustify::Type::Center), NumberBoxWidth(0),
							   CurrentSliderValue(0),
							   bIsDragging(false), LastMousePosition(), TextColor(FColor::White), InvalidTextColor(FColor::Red)
{
}

float ULimenSlider::GetValue()
{
	return CurrentSliderValue;
}

void ULimenSlider::SetValue(const float NewValue)
{
	// Clamp the value within the allowed range
	CurrentSliderValue = FMath::Clamp(NewValue, SliderMinValue, SliderMaxValue);

	// Update the UI elements if they exist
	if (SliderImage.IsValid())
	{
		// Calculate the fill percentage
		const float FillPercent = (CurrentSliderValue - SliderMinValue) / (SliderMaxValue - SliderMinValue);
		
		// Scale the image horizontally (fill effect)
		SliderImage->SetRenderTransform(FSlateRenderTransform(FScale2D(FillPercent, 1.0f)));

	}

	if (NumberText.IsValid())
	{
		NumberText->SetText(GetValueAsText());
	}
}

void ULimenSlider::SetMaxValue(const float NewMax)
{
	SliderMaxValue = FMath::Max(NewMax, SliderMinValue);
	SetValue(CurrentSliderValue);
}

void ULimenSlider::SetMinValue(const float NewMin)
{
	SliderMinValue = FMath::Min(NewMin, SliderMaxValue);
	SetValue(CurrentSliderValue);
}

TSharedRef<SWidget> ULimenSlider::RebuildWidget()
{
	// return Super::RebuildWidget();

	InputDetector = SNew(SMouseDetector)
		.OnMouseButtonDown(BIND_UOBJECT_DELEGATE(FPointerEventHandler, OnPressed))
		.OnMouseButtonUp(BIND_UOBJECT_DELEGATE(FPointerEventHandler, OnReleased))
		.OnMouseMove(BIND_UOBJECT_DELEGATE(FPointerEventHandler, OnMoved));
	
	SliderImage = SNew(SImage)
		.Image(&SliderBrush);

	NumberText = SNew(SEditableText)
		.Font(NumberTextFont)
		.Justification(NumberTextJustification)
		.Text(GetValueAsText())
		.ColorAndOpacity(FSlateColor(FColor::White))
		.OnTextChanged(BIND_UOBJECT_DELEGATE(FOnTextChanged, TextValueChanged))
		.OnTextCommitted(BIND_UOBJECT_DELEGATE(FOnTextCommitted, TextValueCommited));
	
	Root = SNew(SOverlay)
		+SOverlay::Slot() // Background
		[
			SNew(SImage)
			.Image(&Background)
		]
		+SOverlay::Slot() // Slider
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.SizeParam(FStretch())
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[
				SNew(SBorder)
				.Padding(BorderPadding)
				.BorderImage(&BorderBrush)
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				[
					SNew(SOverlay)
					+SOverlay::Slot()
					.HAlign(HAlign_Fill)
					.VAlign(VAlign_Fill)
					.Padding(SliderPadding)
					[					
						SliderImage.ToSharedRef()
					]
					+SOverlay::Slot()
					.HAlign(HAlign_Fill)
					.VAlign(VAlign_Fill)
					[
						InputDetector.ToSharedRef()
					]
				]
			]
			+SHorizontalBox::Slot()
			.Padding(NumberTextMargin)
			.SizeParam(FAuto())
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			[
				SNew(SBox)
				.WidthOverride(NumberBoxWidth)
				[
					NumberText.ToSharedRef()
				]
			]
		];

	return Root.ToSharedRef();
}

void ULimenSlider::ReleaseSlateResources(const bool bReleaseChildren)
{
	InputDetector.Reset();
	SliderImage.Reset();
	NumberText.Reset();
	Root.Reset();

	Super::ReleaseSlateResources(bReleaseChildren);
}

FReply ULimenSlider::OnPressed(const FGeometry& InGeometry, const FPointerEvent& Event)
{
	if (Event.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		bIsDragging = true;
		LastMousePosition = Event.GetScreenSpacePosition();
		return FReply::Handled().CaptureMouse(InputDetector.ToSharedRef());
	}

	return FReply::Unhandled();
}

FReply ULimenSlider::OnReleased(const FGeometry& InGeometry, const FPointerEvent& Event)
{
	if (bIsDragging && Event.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		bIsDragging = false;
		return FReply::Handled().ReleaseMouseCapture();
	}

	return FReply::Unhandled();
}

FReply ULimenSlider::OnMoved(const FGeometry& InGeometry, const FPointerEvent& Event)
{
	if (bIsDragging)
	{
		// Calculate the delta (how much the mouse has moved since the last frame)
		const FVector2D MouseDelta = Event.GetScreenSpacePosition() - LastMousePosition;
		
		// Update the slider value based on the delta
		// You might want to scale the movement for more precision
		UpdateSliderPosition(InGeometry, MouseDelta.X);

		// Update the last mouse position to the current one
		LastMousePosition = Event.GetScreenSpacePosition();

		return FReply::Handled();
	}

	return FReply::Unhandled();
}

void ULimenSlider::TextValueChanged(const FText& InText)
{
	if (InText.IsEmpty())
	{
		NumberText->SetColorAndOpacity(TextColor);
	}
	else
	{
		NumberText->SetColorAndOpacity(ValidateTextInput(InText.ToString()) ? TextColor : InvalidTextColor);
	}
}

void ULimenSlider::TextValueCommited(const FText& InText, const ETextCommit::Type CommitType)
{
	if (!InText.IsNumeric())
	{
		SetValue(CurrentSliderValue);
		return;
	}

	const FString NewValueString = InText.ToString();
	if (ValidateTextInput(NewValueString))
	{
		SetValue(CurrentSliderValue);
		return;
	}
	
	const float NewValue = FCString::Atof(*NewValueString);
	SetValue(FMath::Clamp(NewValue, SliderMinValue, SliderMaxValue));
}

bool ULimenSlider::ValidateTextInput(const FString& InputText) const
{
	if (InputText.IsEmpty() || !InputText.IsNumeric())
	{
		return false;
	}

	if (const float Value = FCString::Atof(*InputText);
		FMath::IsNaN(Value) || !FMath::IsFinite(Value) || Value > SliderMaxValue || Value < SliderMinValue)
	{
		return false;
	}

	return true;
}

FText ULimenSlider::GetValueAsText() const
{
	FString Result = FString::Printf(TEXT("%.*f"), DecimalDigits, CurrentSliderValue);

	if (const int32 DecimalPointIndex = Result.Find(TEXT("."), ESearchCase::IgnoreCase);
		DecimalPointIndex != INDEX_NONE)
	{
		if (Result.Len() > DecimalPointIndex + DecimalDigits)
		{
			Result = Result.Left(DecimalPointIndex + DecimalDigits + 1); // +1 to keep the dot
		}
	}

	return FText::FromString(Result);
}

void ULimenSlider::UpdateSliderPosition(const FGeometry& MyGeometry, const float DeltaX)
{
	const float SliderWidth = MyGeometry.GetLocalSize().X;
	if (SliderWidth <= 0.f) return; // Prevent division by zero

	const float SliderDelta = DeltaX / SliderWidth;
	const float NewValue = FMath::Clamp(CurrentSliderValue + SliderDelta * (SliderMaxValue - SliderMinValue),
										SliderMinValue,
										SliderMaxValue);

	SetValue(NewValue);
}
