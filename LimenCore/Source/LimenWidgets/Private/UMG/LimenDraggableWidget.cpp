// Copyright FaceSoftware. All Rights Reserved.


#include "UMG/LimenDraggableWidget.h"

#include "RenderTransform.h"
#include "Blueprint/UserWidget.h"
#include "Slate/SMouseDetector.h"
#include "Widgets/SOverlay.h"
#include "Widgets/Images/SImage.h"


TSharedRef<SWidget> ULimenDraggableWidget::RebuildWidget()
{
	// return Super::RebuildWidget();

	CustomWidgetInstance.Reset();

	TSharedPtr<SWidget> WidgetIcon;
	switch (WidgetType)
	{
	case EDraggableWidgetType::Brush:
		{
			WidgetIcon = SNew(SImage)
				.Image(&WidgetBrush);
		}
		break;

	case EDraggableWidgetType::CustomWidget:
		{
			UUserWidget* NewWidget = NewObject<UUserWidget>(this, CustomWidget, TEXT("DraggableCustomWidget"), RF_Transient | RF_Transactional);
			CustomWidgetInstance = TStrongObjectPtr(NewWidget);
			CustomWidgetInstance->SetPlayerContext(FLocalPlayerContext(GetOwningLocalPlayer(), GetWorld()));
			CustomWidgetInstance->Initialize();
			WidgetIcon = CustomWidgetInstance->TakeWidget();
		}
		break;

		default: checkNoEntry()
	}

	MouseDetector = SNew(SMouseDetector)
		.OnMouseButtonDown_UObject(this, &ThisClass::MouseDown)
		.OnMouseButtonUp_UObject(this, &ThisClass::MouseUp)
		.OnMouseMove_UObject(this, &ThisClass::MouseMove);

	TSharedRef<SOverlay> Root = SNew(SOverlay);
	Root->AddSlot()
	.HAlign(HorizontalAlignment)
	.VAlign(VerticalAlignment)
	[
		WidgetIcon.ToSharedRef()
	];
	Root->AddSlot()
	.HAlign(HAlign_Fill)
	.VAlign(VAlign_Fill)
	[
		MouseDetector.ToSharedRef()
	];

	return Root;
}

void ULimenDraggableWidget::ReleaseSlateResources(const bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	CustomWidgetInstance.Reset();
	MouseDetector.Reset();
}

FReply ULimenDraggableWidget::MouseDown(const FGeometry& InGeometry, const FPointerEvent& MouseEvent)
{
	const TSharedRef<SWidget> MouseDetectorRef = MouseDetector.ToSharedRef();
	return FReply::Handled().CaptureMouse(MouseDetectorRef).DetectDrag(MouseDetectorRef, MouseEvent.GetEffectingButton());
}

FReply ULimenDraggableWidget::MouseUp(const FGeometry& InGeometry, const FPointerEvent& MouseEvent)
{
	return FReply::Handled().ReleaseMouseCapture();
}

FReply ULimenDraggableWidget::MouseMove(const FGeometry& InGeometry, const FPointerEvent& MouseEvent)
{
	if (MouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		SetRenderTranslation(GetRenderTransform().Translation + MouseEvent.GetCursorDelta());

		return FReply::Handled();
	}

	return FReply::Unhandled();
}