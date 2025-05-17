// Copyright © 2024 FaceSoftware. All rights reserved.


#include "LimenSlider.h"

#include "TimerManager.h"
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
							   SliderPadding(FMargin(4.f)), bUseValueText(true), TextColor(FColor::White),
							   InvalidTextColor(FColor::Yellow), NumberTextJustification(ETextJustify::Type::Center),
							   NumberBoxWidth(100.f), CurrentSliderValue(InitialSliderValue), bIsDragging(false),
							   bIsHovering(false), LastMousePosition(), PreviousMouseCursor(),
							   bShouldRevertCursorIcon(false), bBroadcastValueSetDelegate(false),
							   LastInputType(ELimenSliderInput::Undefined)
{
	InitialSliderValue = SliderMinValue;

	BackgroundBrush.DrawAs = ESlateBrushDrawType::Type::Image;
	BackgroundBrush.Margin = FMargin();
	BackgroundBrush.Tiling = ESlateBrushTileType::Type::NoTile;
	BackgroundBrush.ImageType = ESlateBrushImageType::Type::FullColor;
	BackgroundBrush.ImageSize = FVector2D(0.f, 0.f);
	BackgroundBrush.TintColor = FSlateColor(FLinearColor::Transparent);

	BackgroundHoveredBrush.DrawAs = ESlateBrushDrawType::Type::Image;
	BackgroundHoveredBrush.Margin = FMargin();
	BackgroundHoveredBrush.Tiling = ESlateBrushTileType::Type::NoTile;
	BackgroundHoveredBrush.ImageType = ESlateBrushImageType::Type::FullColor;
	BackgroundHoveredBrush.ImageSize = FVector2D(0.f, 0.f);
	BackgroundHoveredBrush.TintColor = FLinearColor(1.f, 1.f, 1.f, .3f);

	BorderBrush.DrawAs = ESlateBrushDrawType::Type::RoundedBox;
	BorderBrush.Margin = FMargin();
	BorderBrush.Tiling = ESlateBrushTileType::Type::NoTile;
	BorderBrush.ImageType = ESlateBrushImageType::Type::FullColor;
	BorderBrush.ImageSize = FVector2D(0.f, 0.f);
	BorderBrush.TintColor = FLinearColor::Transparent;
	BorderBrush.OutlineSettings.Color = FLinearColor::White;
	BorderBrush.OutlineSettings.Width = 1.f;
	BorderBrush.OutlineSettings.RoundingType = ESlateBrushRoundingType::FixedRadius;
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

void ULimenSlider::SetDecimalDigitsCount(const int32 NewDecimalDigits)
{
	DecimalDigits = NewDecimalDigits;
	SetValueInternal(ELimenSliderInput::Undefined, CurrentSliderValue, false);
}

int32 ULimenSlider::GetDecimalDigitsCount() const
{
	return DecimalDigits;
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

	SliderBackground = SNew(SImage)
		.Image(&BackgroundHoveredBrush)
		.ColorAndOpacity(FSlateColor(FColor::Transparent));

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
			.Padding(0.f)
			[
				SliderBackground.ToSharedRef()
			]
			+SOverlay::Slot()
			.Padding(SliderPadding)
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
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
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[
				NumberText.ToSharedRef()
			]
		];
	}

	Root = SNew(SOverlay)
		+SOverlay::Slot() // Background
		[
			SNew(SImage).Image(&BackgroundBrush)
		]
		+SOverlay::Slot() // Slider
		[
			HorizontalBox
		];

	SetValueInternal(ELimenSliderInput::Undefined, InitialSliderValue, false);

	return Root.ToSharedRef();
}

void ULimenSlider::ReleaseSlateResources(const bool bReleaseChildren)
{
	InputDetector.Reset();
	SliderImage.Reset();
	NumberText.Reset();
	SliderBackground.Reset();
	Root.Reset();

	Super::ReleaseSlateResources(bReleaseChildren);
}

void ULimenSlider::OnHover(const FGeometry& InGeometry, const FPointerEvent& Event)
{
	bIsHovering = true;
	PreviousMouseCursor = GetCursor();
	bShouldRevertCursorIcon = false;
	SetCursor(EMouseCursor::ResizeLeftRight);
	SliderBackground->SetColorAndOpacity(FSlateColor(FLinearColor::White));
}

void ULimenSlider::OnUnHover(const FPointerEvent& Event)
{
	bIsHovering = false;
	bShouldRevertCursorIcon = true;
	if (!bIsDragging) SliderBackground->SetColorAndOpacity(FSlateColor(FLinearColor::Transparent));
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

		if (!bIsHovering) SliderBackground->SetColorAndOpacity(FSlateColor(FLinearColor::Transparent));
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
				LastMousePosition = Event.GetScreenSpacePosition();
				UpdateSliderPositionWithDelta(InGeometry, (Event.GetScreenSpacePosition() - LastMousePosition).X);
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
		LastInputType = InputType;
		bBroadcastValueSetDelegate = true;

		if (!WorldTickDelegate.IsValid())
		{
			WorldTickDelegate = FWorldDelegates::OnWorldTickEnd.AddUObject(this, &ThisClass::WorldTick);
		}
	}
}

void ULimenSlider::WorldTick(UWorld* WorldPtr, const ELevelTick LevelTick, const float DeltaTime)
{
	if (bBroadcastValueSetDelegate)
	{
		bBroadcastValueSetDelegate = false;
		OnNewValueSet.Broadcast(LastInputType, GetValue());

		verify(FWorldDelegates::OnWorldTickEnd.Remove(WorldTickDelegate));
		WorldTickDelegate.Reset();
	}
}
