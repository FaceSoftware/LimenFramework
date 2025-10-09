// Copyright FaceSoftware. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/Widget.h"
#include "Blueprint/UserWidget.h"
#include "LimenDraggableWidget.generated.h"


class SMouseDetector;

UENUM()
enum class EDraggableWidgetType : uint8
{
	Brush,
	CustomWidget,
};

/**
 * 
 */
UCLASS()
class LIMENWIDGETS_API ULimenDraggableWidget : public UWidget
{
	GENERATED_BODY()

public:

protected:
	UPROPERTY(EditAnywhere, Category="Draggable Widget")
	EDraggableWidgetType WidgetType;
	UPROPERTY(EditAnywhere, Category="Draggable Widget", meta=(EditCondition="WidgetType==EDraggableWidgetType::Brush"))
	TSubclassOf<UUserWidget> CustomWidget;
	UPROPERTY(EditAnywhere, Category="Draggable Widget", meta=(EditCondition="WidgetType==EDraggableWidgetType::Brush"))
	FSlateBrush WidgetBrush;
	UPROPERTY(EditAnywhere, Category="Draggable Widget")
	TEnumAsByte<EHorizontalAlignment> HorizontalAlignment;
	UPROPERTY(EditAnywhere, Category="Draggable Widget")
	TEnumAsByte<EVerticalAlignment> VerticalAlignment;

	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void ReleaseSlateResources(const bool bReleaseChildren) override;

	virtual FReply MouseDown(const FGeometry& InGeometry, const FPointerEvent& MouseEvent);
	virtual FReply MouseUp(const FGeometry& InGeometry, const FPointerEvent& MouseEvent);
	virtual FReply MouseMove(const FGeometry& InGeometry, const FPointerEvent& MouseEvent);

private:
	TStrongObjectPtr<UUserWidget> CustomWidgetInstance;
	TSharedPtr<SMouseDetector> MouseDetector;
};
