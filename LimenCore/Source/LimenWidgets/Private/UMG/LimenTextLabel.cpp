// Copyright FaceSoftware. All Rights Reserved.


#include "UMG/LimenTextLabel.h"

#include "Blueprint/UserWidget.h"


ULimenTextLabel::ULimenTextLabel()
{
	LabelTitleTextColor = FSlateColor(FColor::White);
	LabelValueTextColor = FSlateColor(FColor::White);
}

void ULimenTextLabel::SetLabelTitle(const FText& InTitle)
{
	LabelTitle = InTitle;
	if (TitleTextBlock.IsValid())
	{
		TitleTextBlock->SetText(LabelTitle);
	}
	
	if (const TSharedPtr<SWidget> SafeWidget = GetCachedWidget())
	{
		SafeWidget->Invalidate(EInvalidateWidgetReason::Volatility);
	}
}

void ULimenTextLabel::SetLabelTitleFont(const FSlateFontInfo& InTitleFont)
{
	LabelTitleFont = InTitleFont;
	if (TitleTextBlock.IsValid())
	{
		TitleTextBlock->SetFont(LabelTitleFont);
	}
	
	if (const TSharedPtr<SWidget> SafeWidget = GetCachedWidget())
	{
		SafeWidget->Invalidate(EInvalidateWidgetReason::Volatility);
	}
}

void ULimenTextLabel::SetLabelTitlePadding(const FMargin& InPadding)
{
	LabelTitlePadding = InPadding;
	if (const TSharedPtr<SWidget> SafeWidget = GetCachedWidget())
	{
		SafeWidget->Invalidate(EInvalidateWidgetReason::Layout);
	}
}

void ULimenTextLabel::SetLabelTitleTextColor(const FSlateColor& InColor)
{
	LabelTitleTextColor = InColor;
	if (TitleTextBlock.IsValid())
	{
		TitleTextBlock->SetColorAndOpacity(LabelTitleTextColor);
	}
	if (const TSharedPtr<SWidget> SafeWidget = GetCachedWidget())
	{
		SafeWidget->Invalidate(EInvalidateWidgetReason::Paint);
	}
}

void ULimenTextLabel::SetLabelValue(const FText& InValue)
{
	LabelValue = InValue;
	if (ValueTextBlock.IsValid())
	{
		ValueTextBlock->SetText(LabelValue);
	}
	
	if (const TSharedPtr<SWidget> SafeWidget = GetCachedWidget())
	{
		SafeWidget->Invalidate(EInvalidateWidgetReason::Volatility);
	}
}

void ULimenTextLabel::SetLabelValueFont(const FSlateFontInfo& InValueFont)
{
	LabelValueFont = InValueFont;
	if (ValueTextBlock.IsValid())
	{
		ValueTextBlock->SetFont(LabelTitleFont);
	}
	
	if (const TSharedPtr<SWidget> SafeWidget = GetCachedWidget())
	{
		SafeWidget->Invalidate(EInvalidateWidgetReason::Volatility);
	}
}

void ULimenTextLabel::SetLabelValuePadding(const FMargin& InPadding)
{
	LabelTitlePadding = InPadding;
	if (const TSharedPtr<SWidget> SafeWidget = GetCachedWidget())
	{
		SafeWidget->Invalidate(EInvalidateWidgetReason::Layout);
	}
}

void ULimenTextLabel::SetLabelValueTextColor(const FSlateColor& InColor)
{
	LabelValueTextColor = InColor;
	if (ValueTextBlock.IsValid())
	{
		ValueTextBlock->SetColorAndOpacity(LabelValueTextColor);
	}
	if (const TSharedPtr<SWidget> SafeWidget = GetCachedWidget())
	{
		SafeWidget->Invalidate(EInvalidateWidgetReason::Paint);
	}
}

void ULimenTextLabel::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);
	
	TitleTextBlock.Reset();
	ValueTextBlock.Reset();
}

TSharedRef<SWidget> ULimenTextLabel::RebuildWidget()
{
	// return Super::RebuildWidget();
	
	TitleTextBlock = SNew(STextBlock)
		.Text(LabelTitle)
		.Font(LabelTitleFont)
		.ColorAndOpacity(LabelTitleTextColor);
	
	ValueTextBlock = SNew(STextBlock)
		.Text(LabelValue)
		.Font(LabelValueFont)
		.ColorAndOpacity(LabelValueTextColor);
	
	TSharedRef<SHorizontalBox> Root = SNew(SHorizontalBox)
		+SHorizontalBox::Slot()
		.Padding(LabelTitlePadding)
		.VAlign(LabelTitleVerticalAlignment)
		.HAlign(LabelTitleHorizontalAlignment)
		[
			TitleTextBlock.ToSharedRef()
		]
		+SHorizontalBox::Slot()
		.VAlign(LabelValueVerticalAlignment)
		.HAlign(LabelValueHorizontalAlignment)
		.Padding(LabelValuePadding)
		[
			ValueTextBlock.ToSharedRef()
		];
	
	return Root;
}


///
///
///


ULimenLabel::ULimenLabel()
{
}

void ULimenLabel::SetLabelTitle(const FText& InTitle)
{
}

void ULimenLabel::SetLabelTitleFont(const FSlateFontInfo& InTitleFont)
{
}

void ULimenLabel::SetLabelTitlePadding(const FMargin& InPadding)
{
}

void ULimenLabel::SetLabelValue(UUserWidget* InWidget)
{
	LabelValueWidget = TStrongObjectPtr(InWidget);
	InvalidateLayoutAndVolatility();
}

UUserWidget* ULimenLabel::GetLabelValue() const
{
	return LabelValueWidget.Get();
}

UUserWidget* ULimenLabel::GetLabelValue(TSubclassOf<UUserWidget> Class) const
{
	return LabelValueWidget.Get();
}

void ULimenLabel::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);
	TitleTextBlock.Reset();
}

TSharedRef<SWidget> ULimenLabel::RebuildWidget()
{
	// return Super::RebuildWidget();

	if (LabelValue)
	{
		auto* ValueWidget = UUserWidget::CreateWidgetInstance(*this, LabelValue, NAME_None);
		LabelValueWidget = TStrongObjectPtr(ValueWidget);
	}
	
	TitleTextBlock = SNew(STextBlock)
		.Text(LabelTitle)
		.Font(LabelTitleFont);
	
	TSharedRef<SHorizontalBox> Root = SNew(SHorizontalBox)
		+SHorizontalBox::Slot()
		.Padding(LabelTitlePadding)
		.VAlign(LabelTitleVerticalAlignment)
		.HAlign(LabelValueHorizontalAlignment)
		[
			TitleTextBlock.ToSharedRef()
		]
		+SHorizontalBox::Slot()
		.Padding(LabelValuePadding)
		.VAlign(LabelValueVerticalAlignment)
		.HAlign(LabelValueHorizontalAlignment)
		[
			LabelValueWidget.IsValid() ? LabelValueWidget->TakeWidget() : SNew(SSpacer)
		];
	
	return Root;
}
