// Copyright Face Software. All Rights Reserved.


#include "Widgets/LimenWidget.h"

#include "Blueprint/WidgetTree.h"
#include "LogMacros/LimenLogMacros.h"


ULimenWidget::ULimenWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	SetIsFocusable(false);
	DefaultVisibleState = ESlateVisibility::Visible;
	DefaultHiddenState = ESlateVisibility::Collapsed;
	WidgetLevel = 0;
}

void ULimenWidget::NativeConstruct()
{
	Super::NativeConstruct();

	TakeWidget()->SetCanTick(true);
	OnLimenAnimationFinished.AddUniqueDynamic(this, &ThisClass::WidgetAnimationEnd_Internal);
	OnLimenVisibilityChanged.AddUniqueDynamic(this, &ThisClass::WidgetVisibilityChanged_Internal);
}

void ULimenWidget::ShowWidget()
{
	if (bIsAnimating)
	{
		return;
	}
	
	if (!IsInViewport() && !GetParent())
	{
		ShowWidgetMethod();
		LIMEN_LOG(LogLimenCore, Log, this, "Widget added to viewport with ZOrder = %d", WidgetLevel);
	}
	else if (IsShowing())
	{
		return;
	}
	
	SetVisibility(DefaultVisibleState);
	// ShowAllChildren();
	OnWidgetVisible();
	OnLimenVisibilityChanged.Broadcast(true);
	
	if (bUseShowAnimation)
	{
		PlayOnWidgetVisibleAnimation();
		bIsAnimating = true;
	}
	else
	{
		NotifyAnimationFinished(true);
	}
}

void ULimenWidget::HideWidget()
{
	if (bIsAnimating)
	{
		return;
	}
	
	if (IsHiding())
	{
		return;
	}

	OnLimenVisibilityChanged.Broadcast(false);
	
	if (bUseHideAnimation)
	{
		PlayOnWidgetHiddenAnimation();
		bIsAnimating = true;
	}
	else
	{
		NotifyAnimationFinished(false);
	}
}

void ULimenWidget::HideWidgetWithCallback(FLimenBlueprintWidgetHidden OnWidgetHidden)
{
	if (!IsShowing() || !bUseHideAnimation)
	{
		if (OnWidgetHidden.IsBound())
		{
			OnWidgetHidden.Execute();
		}
		
		return;
	}

	HideWidget();
	CurrentBlueprintDelegate = OnWidgetHidden;
}

void ULimenWidget::HideWidget(const FLimenWidgetHidden& OnWidgetHidden)
{
	if (!IsShowing() || !bUseHideAnimation)
	{
		if (OnWidgetHidden.IsBound())
		{
			OnWidgetHidden.Execute();
		}
		
		return;
	}

	HideWidget();
	CurrentDelegate = OnWidgetHidden;
}

void ULimenWidget::ToggleWidgetVisibility()
{
	if (IsShowing())
	{
		HideWidget();
	}
	else
	{
		ShowWidget();
	}
}

bool ULimenWidget::IsShowing() const
{	
	return GetVisibility() == DefaultVisibleState;
}

bool ULimenWidget::IsHiding() const
{
	return GetVisibility() == DefaultHiddenState;
}

bool ULimenWidget::IsAnimating() const
{
	return bIsAnimating;
}

void ULimenWidget::DestroyWidget(const bool bWaitForHideAnimation)
{
	if (!bWaitForHideAnimation)
	{
		DestroyWidgetInternal();
		return;
	}

	OnLimenAnimationFinished.AddUniqueDynamic(this, &ThisClass::DestroyWidgetInternal);
	HideWidget();
}

void ULimenWidget::NotifyAnimationFinished(const bool bIsVisibleAnimation)
{
	bIsAnimating = false;
	
	if (bIsVisibleAnimation)
	{
		// Nothing to do here, if it's the visible animation the logic should go on the "ShowWidget" function
	}
	else
	{
		// HideAllChildren();
		HideWidgetMethod();
		OnWidgetHidden();
	}

	OnLimenAnimationFinished.Broadcast(bIsVisibleAnimation);
}

void ULimenWidget::SetWidgetLevel(const int32 NewLevel)
{
	WidgetLevel = NewLevel;
}

uint8 ULimenWidget::GetWidgetLevel() const
{
	return WidgetLevel;
}

void ULimenWidget::SetDefaultVisibleState(const ESlateVisibility NewDefaultVisibleState)
{
	DefaultVisibleState = NewDefaultVisibleState;
}

void ULimenWidget::SetDefaultHiddenState(const ESlateVisibility NewDefaultHiddenState)
{
	DefaultHiddenState = NewDefaultHiddenState;
}

ESlateVisibility ULimenWidget::GetDefaultVisibleState() const
{
	return DefaultVisibleState;
}

ESlateVisibility ULimenWidget::GetDefaultHiddenState() const
{
	return DefaultHiddenState;
}

void ULimenWidget::ShowWidgetMethod()
{
	AddToViewport(WidgetLevel);
}

void ULimenWidget::HideWidgetMethod()
{
	SetVisibility(DefaultHiddenState);
}

void ULimenWidget::ShowAllChildren()
{
	TArray<UWidget*> Widgets;
	WidgetTree->GetAllWidgets(Widgets);
	for (UWidget* Widget : Widgets)
	{
		auto* Temp = Cast<ULimenWidget>(Widget);
		if (!Temp)
		{
			continue;
		}
		
		Temp->ShowWidget();
	}
}

void ULimenWidget::HideAllChildren()
{
	if (!WidgetTree)
	{
		return;
	}

	
	TArray<UWidget*> Widgets;
	WidgetTree->GetChildWidgets(this, Widgets);
	for (const auto Widget : Widgets)
	{
		auto* Temp = Cast<ULimenWidget>(Widget);
		if (!Temp)
		{
			continue;
		}
		
		Temp->HideAllChildren();
		Temp->HideWidget();
	}
}

void ULimenWidget::WidgetAnimationEnd_Internal(const bool bVisible)
{
	if (bVisible)
	{
	}
	else
	{
		if (CurrentBlueprintDelegate.IsBound())
		{
			CurrentBlueprintDelegate.Execute();
			CurrentBlueprintDelegate.Unbind();
		}
		if (CurrentDelegate.IsBound())
		{
			CurrentDelegate.Execute();
			CurrentDelegate.Unbind();
		}
	}
	
}

void ULimenWidget::WidgetVisibilityChanged_Internal(const bool bVisible)
{
}

void ULimenWidget::DestroyWidgetInternal(const bool bIsHideAnimation)
{
	if (!bIsHideAnimation)
	{
		return;
	}
	
	RemoveFromParent();
	ConditionalBeginDestroy();
}
