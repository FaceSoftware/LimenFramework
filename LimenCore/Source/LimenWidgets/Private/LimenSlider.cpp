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


ULimenSlider::ULimenSlider() : Super(), SliderInputMethod(ELimenSliderInputMethod::MousePosition),
							   InitialSliderValue(0.f), DecimalDigits(1), SliderMinValue(0), SliderMaxValue(1),
							   Background(static_cast<FSlateBrush>(FDefaultBackgroundBrush())),
							   BorderBrush(static_cast<FSlateBrush>(FDefaultBorderBrush())), SliderPadding(4.f),
							   bUseValueText(true), TextColor(FColor::White), InvalidTextColor(FColor::Yellow),
							   NumberTextJustification(ETextJustify::Type::Center), NumberBoxWidth(100.f),
							   CurrentSliderValue(InitialSliderValue), bIsDragging(false), LastMousePosition(),
							   PreviousMouseCursor(), bShouldRevertCursorIcon(false)
{
	InitialSliderValue = SliderMinValue;
}

float ULimenSlider::GetValue() const
{
	return CurrentSliderValue;
}

void ULimenSlider::SetValue(const float NewValue)
{
	SetValueInternal(ELimenSliderInput::Undefined, NewValue, false);
}

void ULimenSlider::SetMaxValue(const float NewMax)
{
	SliderMaxValue = FMath::Max(NewMax, SliderMinValue);
	SetValueInternal(ELimenSliderInput::Undefined, CurrentSliderValue, false);
}

void ULimenSlider::SetMinValue(const float NewMin)
{
	SliderMinValue = FMath::Min(NewMin, SliderMaxValue);
	SetValueInternal(ELimenSliderInput::Undefined, CurrentSliderValue, false);
}

TSharedRef<SWidget> ULimenSlider::RebuildWidget()
{
	// return Super::RebuildWidget();

	InputDetector = SNew(SMouseDetector)
		.OnMouseButtonDown(BIND_UOBJECT_DELEGATE(FPointerEventHandler, OnPressed))
		.OnMouseButtonUp(BIND_UOBJECT_DELEGATE(FPointerEventHandler, OnReleased))
		.OnMouseMove(BIND_UOBJECT_DELEGATE(FPointerEventHandler, OnMoved))
		.OnMouseHover(BIND_UOBJECT_DELEGATE(FNoReplyPointerEventHandler, OnHover))
		.OnMouseUnHover(BIND_UOBJECT_DELEGATE(FSimpleNoReplyPointerEventHandler, OnUnHover));
	
	SliderImage = SNew(SImage)
		.Image(&SliderBrush);

	const TSharedRef<SHorizontalBox> HorizontalBox = SNew(SHorizontalBox)
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
	];

	if (bUseValueText)
	{
		NumberText = SNew(SEditableText)
			.Font(NumberTextFont)
			.Justification(NumberTextJustification)
			.Text(GetValueAsText())
			.ColorAndOpacity(FSlateColor(FColor::White))
			.OnTextChanged(BIND_UOBJECT_DELEGATE(FOnTextChanged, TextValueChanged))
			.OnTextCommitted(BIND_UOBJECT_DELEGATE(FOnTextCommitted, TextValueCommited));

		HorizontalBox->AddSlot()
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
		];
	}
	
	Root = SNew(SOverlay)
		+SOverlay::Slot() // Background
		[
			SNew(SImage)
			.Image(&Background)
		]
		+SOverlay::Slot() // Slider
		[
			HorizontalBox
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

void ULimenSlider::OnHover(const FGeometry& InGeometry, const FPointerEvent& Event)
{
	PreviousMouseCursor = GetCursor();
	bShouldRevertCursorIcon = false;
	SetCursor(EMouseCursor::ResizeLeftRight);
}

void ULimenSlider::OnUnHover(const FPointerEvent& Event)
{
	bShouldRevertCursorIcon = true;
}

FReply ULimenSlider::OnPressed(const FGeometry& InGeometry, const FPointerEvent& Event)
{
	if (Event.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		bIsDragging = true;

		switch (SliderInputMethod)
		{
		case ELimenSliderInputMethod::MousePosition:
			{
				const FVector2D& MouseScreenPosition = Event.GetScreenSpacePosition();
				const float LocalMouseX = InGeometry.AbsoluteToLocal(MouseScreenPosition).X;
				UpdateSliderPositionWithCursorPosition(InGeometry, LocalMouseX);
			}
			break;

		default:
			break;
		}
		
		return FReply::Handled().CaptureMouse(InputDetector.ToSharedRef());
	}

	return FReply::Unhandled();
}

FReply ULimenSlider::OnReleased(const FGeometry& InGeometry, const FPointerEvent& Event)
{
	if (bIsDragging && Event.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		bIsDragging = false;
		if (bShouldRevertCursorIcon) SetCursor(PreviousMouseCursor);
		return FReply::Handled().ReleaseMouseCapture();
	}

	return FReply::Unhandled();
}

FReply ULimenSlider::OnMoved(const FGeometry& InGeometry, const FPointerEvent& Event)
{
	if (bIsDragging)
	{
		switch (SliderInputMethod)
		{
		case ELimenSliderInputMethod::Drag:
			{
				UpdateSliderPositionWithDelta(InGeometry, (Event.GetScreenSpacePosition() - LastMousePosition).X);
				LastMousePosition = Event.GetScreenSpacePosition();
			}
			break;

		case ELimenSliderInputMethod::MousePosition:
			{
				const FVector2D& MouseScreenPosition = Event.GetScreenSpacePosition();
				const float LocalMouseX = InGeometry.AbsoluteToLocal(MouseScreenPosition).X;
				UpdateSliderPositionWithCursorPosition(InGeometry, LocalMouseX);
			}
			break;
		}

		return FReply::Handled();
	}

	return FReply::Unhandled();
}

void ULimenSlider::TextValueChanged(const FText& InText)
{
	FString InputString = InText.ToString();
	if (InputString.IsEmpty())
	{
		NumberText->SetColorAndOpacity(TextColor);
		return;
	}
	
	if (InputString.IsNumeric())
	{
		NumberText->SetColorAndOpacity(ValidateTextInput(InputString) ? TextColor : InvalidTextColor);
	}
	else
	{
		InputString.RemoveAt(InputString.Len() - 1);
		NumberText->SetText(FText::FromString(InputString));
	}
}

void ULimenSlider::TextValueCommited(const FText& InText, const ETextCommit::Type CommitType)
{
	if (!InText.IsNumeric())
	{
		SetValueInternal(ELimenSliderInput::Undefined, CurrentSliderValue, false);
		return;
	}

	const FString NewValueString = InText.ToString();
	if (!ValidateTextInput(NewValueString))
	{
		SetValueInternal(ELimenSliderInput::Undefined, CurrentSliderValue, false);
		return;
	}
	
	const float NewValue = FCString::Atof(*NewValueString);
	SetValueInternal(ELimenSliderInput::Typed, FMath::Clamp(NewValue, SliderMinValue, SliderMaxValue), true);
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

void ULimenSlider::UpdateSliderPositionWithDelta(const FGeometry& MyGeometry, const float DeltaX)
{
	const float SliderWidth = MyGeometry.GetLocalSize().X;
	if (FMath::IsNearlyZero(SliderWidth)) return; // Prevent division by zero

	const float SliderDelta = DeltaX / SliderWidth;
	const float NewValue = FMath::Clamp(CurrentSliderValue + SliderDelta * (SliderMaxValue - SliderMinValue),
										SliderMinValue,
										SliderMaxValue);

	SetValueInternal(ELimenSliderInput::MouseDrag, NewValue, true);
}

void ULimenSlider::UpdateSliderPositionWithCursorPosition(const FGeometry& MyGeometry, const float LocalMouseX)
{
	const float SliderWidth = MyGeometry.GetLocalSize().X;
	if (SliderWidth <= 0.f) return; // Prevent division by zero

	const float NormalizedValue = FMath::Clamp(LocalMouseX / SliderWidth, 0.0f, 1.0f);

	const float NewValue = FMath::Lerp(SliderMinValue, SliderMaxValue, NormalizedValue);
	SetValueInternal(ELimenSliderInput::MouseDrag, NewValue, true);
}

void ULimenSlider::SetValueInternal(const ELimenSliderInput InputType, const float Value, const bool bShouldBroadcast)
{
	CurrentSliderValue = FMath::Clamp(Value, SliderMinValue, SliderMaxValue);

	if (SliderImage.IsValid())
	{
		const float FillPercent = (CurrentSliderValue - SliderMinValue) / (SliderMaxValue - SliderMinValue);
		SliderImage->SetRenderTransform(FSlateRenderTransform(FScale2D(FillPercent, 1.0f)));
	}

	if (NumberText.IsValid())
	{
		NumberText->SetText(GetValueAsText());
	}

	if (bShouldBroadcast)
	{
		check(InputType != ELimenSliderInput::Undefined)
		OnNewValueSet.Broadcast(InputType, Value);
	}
}
