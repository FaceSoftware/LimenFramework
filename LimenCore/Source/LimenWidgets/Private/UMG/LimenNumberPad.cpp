// Copyright FaceSoftware. All rights reserved.


#include "UMG/LimenNumberPad.h"

#include "UMG/Buttons/LimenStandardButton.h"

void ULimenNumberPad::ClearScreen()
{
	FString CurrentText = TEXT("");
	for (int i = 0; i < MaxDigits; ++i)
	{
		CurrentText += TEXT("-");
	}
	Screen->SetText(FText::FromString(CurrentText));
}

TSharedRef<SWidget> ULimenNumberPad::RebuildWidget()
{
	// return Super::RebuildWidget();

	
	TSharedRef<SGridPanel> GridPanel = SNew(SGridPanel);
	GridPanel->SetRowFill(0, 2.f);
	GridPanel->SetRowFill(1, 1.f);
	GridPanel->SetRowFill(2, 1.f);
	GridPanel->SetRowFill(3, 1.f);
	GridPanel->SetRowFill(4, 1.f);
	
	GridPanel->SetColumnFill(0, 1.f);
	GridPanel->SetColumnFill(1, 1.f);
	GridPanel->SetColumnFill(2, 1.f);

	{
		Screen = SNew(STextBlock);
		Screen->SetFont(ScreenFont);
		ClearScreen();

		auto ScreenOverlay = SNew(SOverlay)
			+SOverlay::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[
				SNew(SImage)
				.Image(&ScreenBrush)
			]
			+SOverlay::Slot()
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			[
				Screen.ToSharedRef()
			];

		auto ScreenSlot = GridPanel->AddSlot(0, 0);
		ScreenSlot.AttachWidget(ScreenOverlay);
		ScreenSlot.ColumnSpan(3);
		ScreenSlot.HAlign(HAlign_Fill);
		ScreenSlot.VAlign(VAlign_Fill);
	}

	// Numbers
	{
		{
			auto* ZeroButton = NewObject<ULimenStandardButton>();
			ZeroButton->SetButtonText(FText::FromString(FString::Printf(TEXT("%d"), 0)));
			ZeroButton->SetButtonStyle(ButtonStyle);
			ZeroButton->SetButtonFont(ButtonFont);
			ZeroButton->OnButtonClickedEvent.AddUniqueDynamic(this, &ThisClass::NumberPressed);
			NumberButtons.Push(TStrongObjectPtr(ZeroButton));

			auto ZeroSlot = GridPanel->AddSlot(1, 4);
			ZeroSlot.AttachWidget(ZeroButton->TakeWidget());
			ZeroSlot.Padding(ButtonPadding);
		}

		{
			SubmitButton = TStrongObjectPtr(NewObject<ULimenStandardButton>());
			SubmitButton->SetButtonText(SubmitButtonText);
			SubmitButton->SetButtonStyle(ButtonStyle);
			SubmitButton->SetButtonFont(ButtonFont);
			SubmitButton->OnButtonClickedEvent.AddUniqueDynamic(this, &ThisClass::SubmitPressed);

			auto SubmitButtonSlot = GridPanel->AddSlot(0, 4);
			SubmitButtonSlot.AttachWidget(SubmitButton->TakeWidget());
			SubmitButtonSlot.Padding(ButtonPadding);
		}

		{
			ClearButton = TStrongObjectPtr(NewObject<ULimenStandardButton>());
			ClearButton->SetButtonText(ClearButtonText);
			ClearButton->SetButtonStyle(ButtonStyle);
			ClearButton->SetButtonFont(ButtonFont);
			ClearButton->OnButtonClickedEvent.AddUniqueDynamic(this, &ThisClass::ClearPressed);

			auto ClearButtonSlot = GridPanel->AddSlot(2, 4);
			ClearButtonSlot.AttachWidget(ClearButton->TakeWidget());
			ClearButtonSlot.Padding(ButtonPadding);
		}

		for (int y = 0; y < 3; ++y)
		{
			for (int x = 0; x < 3; ++x)
			{
				auto* Button = NewObject<ULimenStandardButton>();
				const int32 ButtonNumber = y * 3 + x + 1;
				Button->SetButtonText(FText::FromString(FString::Printf(TEXT("%d"), ButtonNumber)));
				Button->SetButtonStyle(ButtonStyle);
				Button->SetButtonFont(ButtonFont);
				Button->OnButtonClickedEvent.AddUniqueDynamic(this, &ThisClass::NumberPressed);
				NumberButtons.Push(TStrongObjectPtr(Button));

				auto ButtonSlot = GridPanel->AddSlot(x, y + 1);
				ButtonSlot.AttachWidget(Button->TakeWidget());
				ButtonSlot.Padding(ButtonPadding);
			}
		}
	}

	return GridPanel;
}

void ULimenNumberPad::NumberPressed(ULimenStandardButton* Button)
{
	FString CurrentCode = Screen->GetText().ToString();

	int32 AvailableSlot = INDEX_NONE;
	if (!CurrentCode.FindChar(TEXT('-'), AvailableSlot))
	{
		return;
	}

	const FString ButtonText = Button->GetButtonText().ToString();
	check(ButtonText.Len() == 1);
	CurrentCode[AvailableSlot] = ButtonText[0];

	Screen->SetText(FText::FromString(CurrentCode));
}

void ULimenNumberPad::ClearPressed(ULimenStandardButton* Button)
{
	ClearScreen();
}

void ULimenNumberPad::SubmitPressed(ULimenStandardButton* Button)
{
	OnCodeSubmitted.Broadcast(Screen->GetText().ToString() == Solution);
}
