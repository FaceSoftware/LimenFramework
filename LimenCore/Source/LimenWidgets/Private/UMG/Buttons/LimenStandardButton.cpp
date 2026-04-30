// Copyright Face Software. All Rights Reserved.


#include "UMG/Buttons/LimenStandardButton.h"

#include "Framework/SlateDelegates.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SOverlay.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Text/STextBlock.h"


ULimenStandardButton::ULimenStandardButton() : Super()
{
	Text = FText::FromString(TEXT("ButtonText"));
	TextColor = FColor::White;

	TextHorizontalAlignment = HAlign_Center;
	TextVerticalAlignment = VAlign_Center;

	ButtonId = FGuid::NewGuid();
	TextJustification = ETextJustify::Center;
}

void ULimenStandardButton::ReleaseSlateResources(const bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	TextBlock.Reset();
}

void ULimenStandardButton::SetButtonText(const FText& NewText)
{
	Text = NewText;
	if (TextBlock.IsValid())
	{
		TextBlock->SetText(NewText);
	}
}

void ULimenStandardButton::SetButtonStyle(const FButtonStyle& NewStyle)
{
	ButtonStyle = NewStyle;
}

void ULimenStandardButton::SetButtonNormalBrush(const FSlateBrush& NewBrush)
{
	ButtonStyle.Normal = NewBrush;
}

void ULimenStandardButton::SetButtonFont(const FSlateFontInfo& NewFont)
{
	Font = NewFont;
}

const FGuid& ULimenStandardButton::GetButtonId() const
{
	return ButtonId;
}

const FText& ULimenStandardButton::GetButtonText() const
{
	return TextBlock->GetText();
}

TSharedRef<SWidget> ULimenStandardButton::RebuildWidget()
{
	TextBlock = SNew(STextBlock)
		.Text(Text)
		.ColorAndOpacity(TextColor)
		.Font(Font)
		.Justification(TextJustification)
		.Visibility(EVisibility::HitTestInvisible);
	
	TSharedRef<SOverlay> Root = SNew(SOverlay)
	+ SOverlay::Slot()
	.HAlign(HAlign_Fill)
	.VAlign(VAlign_Fill)
	[
		SNew(SButton)
		.ButtonStyle(&ButtonStyle)
		.OnClicked(BIND_UOBJECT_DELEGATE(FOnClicked, OnClicked))
		.OnHovered(BIND_UOBJECT_DELEGATE(FSimpleDelegate, OnHovered))
		.OnUnhovered(BIND_UOBJECT_DELEGATE(FSimpleDelegate, OnUnhovered))
	]
	+ SOverlay::Slot()
	.HAlign(TextHorizontalAlignment)
	.VAlign(TextVerticalAlignment)
	[
		TextBlock.ToSharedRef()
	];
	
	return Root;
}

FReply ULimenStandardButton::OnClicked()
{
	OnButtonClickedEvent.Broadcast(this);
	
	return FReply::Handled();
}

void ULimenStandardButton::OnHovered()
{
	OnButtonHoveredEvent.Broadcast(this);
}

void ULimenStandardButton::OnUnhovered()
{
	OnButtonUnhoveredEvent.Broadcast(this);
}

