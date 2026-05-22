// Copyright Face Software. All Rights Reserved.


#include "Widgets/LimenWidget.h"

#include "Animation/UMGSequencePlayer.h"
#include "Animation/WidgetAnimation.h"
#include "Blueprint/WidgetTree.h"
#include "Kismet/KismetSystemLibrary.h"
#include "LogMacros/LimenLogMacros.h"


ULimenWidget::ULimenWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	SetIsFocusable(false);
	DefaultVisibleState = ESlateVisibility::Visible;
	DefaultHiddenState = ESlateVisibility::Collapsed;
	SetVisibilityInternal(DefaultHiddenState);
	WidgetLevel = 0;
}

void ULimenWidget::NativeConstruct()
{
	Super::NativeConstruct();

	TakeWidget()->SetCanTick(true);
	OnLimenAnimationFinished.AddUObject(this, &ThisClass::WidgetAnimationEnd_Internal);
	OnLimenVisibilityChanged.AddUObject(this, &ThisClass::WidgetVisibilityChanged_Internal);
}

bool ULimenWidget::ShowWidget()
{
	if (bIsAnimating) return false;
	
	if (!IsInViewport() && !GetParent())
	{
		ShowWidgetMethod();
		LIMEN_LOG(LogLimenCore, Log, this, TEXT("Widget added to viewport with ZOrder = %d"), WidgetLevel);
	}
	else if (IsShowing())
	{
		return false;
	}
	
	SetVisibility(DefaultVisibleState);
	// ShowAllChildren();
	OnWidgetVisible();
	OnLimenVisibilityChanged.Broadcast(true);
	
	if (ShowAnimation)
	{
		PlayOnWidgetVisibleAnimation();
		bIsAnimating = true;
		ShowAnimationHandle = PlayAnimation(ShowAnimation.Get(), 0.f, 1, EUMGSequencePlayMode::Forward, 1.f, false);
		ShowAnimationHandle.GetAnimationState()->GetOnWidgetAnimationFinished().AddUObject(this, &ThisClass::ShowAnimationFinished);
	}
	else
	{
		ShowAnimationFinished();
	}

	return true;
}

bool ULimenWidget::HideWidget()
{
	if (bIsAnimating || IsHiding()) return false;

	OnWidgetHidden();
	OnLimenVisibilityChanged.Broadcast(false);
	
	if (HideAnimation)
	{
		PlayOnWidgetHiddenAnimation();
		bIsAnimating = true;
		HideAnimationHandle = PlayAnimation(HideAnimation.Get(), 0.f, 1, EUMGSequencePlayMode::Forward, 1.f, false);
		HideAnimationHandle.GetAnimationState()->GetOnWidgetAnimationFinished().AddUObject(this, &ThisClass::HideAnimationFinished);
	}
	else
	{
		HideAnimationFinished();
	}

	return true;
}

void ULimenWidget::HideWidgetWithCallback(FLimenBlueprintWidgetHidden OnWidgetHidden)
{
	if (!IsShowing() || !HideAnimation)
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
	if (!IsShowing() || !HideAnimation)
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
	return IsInViewport() && GetVisibility() == DefaultVisibleState;
}

bool ULimenWidget::IsHiding() const
{
	return (GetParent() || IsInViewport()) && GetVisibility() == DefaultHiddenState;
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

	OnLimenAnimationFinished.AddUObject(this, &ThisClass::DestroyWidgetInternal);
	HideWidget();
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
	RemoveFromRoot();
	MarkAsGarbage();
}

void ULimenWidget::ShowAnimationFinished(FWidgetAnimationState& State)
{
	ShowAnimationFinished();
}

void ULimenWidget::ShowAnimationFinished()
{
	bIsAnimating = false;
	OnLimenAnimationFinished.Broadcast(true);
}

void ULimenWidget::HideAnimationFinished(FWidgetAnimationState& State)
{
	HideAnimationFinished();
}

void ULimenWidget::HideAnimationFinished()
{
	bIsAnimating = false;
	// HideAllChildren();
	HideWidgetMethod();
	OnLimenAnimationFinished.Broadcast(false);
}
