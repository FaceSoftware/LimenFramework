// Copyright Face Software. All Rights Reserved.


#include "Widgets/LimenMenuWidget.h"

#include "Blueprint/WidgetTree.h"

ULimenMenuWidget::ULimenMenuWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	DefaultVisibleState = ESlateVisibility::Visible;
	DefaultHiddenState = ESlateVisibility::Collapsed;
	SetWidgetLevel(10);
}

void ULimenMenuWidget::BindPlayerController(APlayerController* InPlayerController)
{	
	TArray<UWidget*> Widgets;
	WidgetTree->GetChildWidgets(this, Widgets);
	for (auto* Widget : Widgets)
	{
		if (Widget->GetClass()->IsChildOf(StaticClass()))
		{
			auto* Child = Cast<ULimenMenuWidget>(Widget);
			check(Child);
			
			Child->BindPlayerController(InPlayerController);
		}
	}

	BoundPlayerController = InPlayerController;
	OnPlayerControllerBound(BoundPlayerController.Get());
}
