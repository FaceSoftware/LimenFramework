// Copyright Face Software. All Rights Reserved.


#include "Objects/LimenHint.h"

#include "Blueprint/UserWidget.h"
#include "Developer/LimenHintsDeveloperSettings.h"
#include "UMG/LimenHintWidget.h"


ULimenHint::ULimenHint()
{
	bIsEnabled = false;
	bHasTriedToShowHint = false;
}

void ULimenHint::Initialize()
{
	const ULimenHintsDeveloperSettings* Settings = GetDefault<ULimenHintsDeveloperSettings>();
	if (Settings->DefaultHintWidget.IsNull())
	{
		return;
	}
	
	HintWidgetInstance = CreateWidget<ULimenHintWidget>(GetWorld(), Settings->DefaultHintWidget.LoadSynchronous());
	check(HintWidgetInstance != nullptr);

	FText Title, Body;
	SetHintText(Title, Body);
	HintWidgetInstance->SetHintText(Title, Body);
	
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

void ULimenHint::SetHintText(FText& OutTitle, FText& OutBody)
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
	HintWidgetInstance->DestroyWidget(true);
	HintWidgetInstance = nullptr;
	ConditionalBeginDestroy();
}
