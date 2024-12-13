// Copyright Face Software. All Rights Reserved.


#include "Objects/LimenHint.h"

#include "UMG/LimenHintWidget.h"


ULimenHint::ULimenHint()
{
	bIsEnabled = false;
	bHasTriedToShowHint = false;
}

void ULimenHint::SetHintWidgetClass(const TSubclassOf<ULimenHintWidget>& InClass)
{
	HintWidgetClass = InClass;
}

void ULimenHint::Initialize()
{
	check(HintWidgetInstance == nullptr);
	check(HintWidgetClass != nullptr);
	
	HintWidgetInstance = CreateWidget<ULimenHintWidget>(GetWorld()->GetFirstPlayerController(), HintWidgetClass);

	PreSetup();
	SetupShowHint();
	SetupHideHint();
}

void ULimenHint::Enable()
{
	if (bIsEnabled)
	{
		return;
	}
	
	bIsEnabled = true;
	if (bHasTriedToShowHint)
	{
		ShowHint();
	}
}

void ULimenHint::Disable()
{
	if (!bIsEnabled)
	{
		return;
	}
	
	bIsEnabled = false;
	HideHint();
}

bool ULimenHint::HasEverBeenVisible() const
{
	return bHasTriedToShowHint;
}

void ULimenHint::SetHintText(FText& OutTitle, FText& OutBody)
{
}

void ULimenHint::PreSetup()
{
}

void ULimenHint::SetupShowHint()
{
}

void ULimenHint::SetupHideHint()
{
}

void ULimenHint::ShowHint()
{
	bHasTriedToShowHint = true;
	
	if (!bIsEnabled)
	{
		return;
	}

	if (HintWidgetInstance == nullptr)
	{
		return;
	}

	if (HintWidgetInstance->IsShowing())
	{
		return;
	}
	
	FText Title, Body;
	SetHintText(Title, Body);
	HintWidgetInstance->SetHintText(Title, Body);
	HintWidgetInstance->ShowWidget();
}

void ULimenHint::HideHint()
{
	if (HintWidgetInstance == nullptr)
	{
		return;
	}

	if (bIsEnabled)
	{
		HintWidgetInstance->HideWidget();
	}
	
	OnHintDismissed.Broadcast();
	HintWidgetInstance->DestroyWidget();
	HintWidgetInstance = nullptr;
}
