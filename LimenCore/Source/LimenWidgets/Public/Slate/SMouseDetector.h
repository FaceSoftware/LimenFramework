// Copyright © 2024 FaceSoftware. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

DECLARE_DELEGATE_RetVal_TwoParams(FReply, FDragDropEventHandler, const FGeometry&, const FDragDropEvent&);

/**
 * 
 */
class LIMENWIDGETS_API SMouseDetector : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SMouseDetector)
	{
	}
		
		SLATE_EVENT( FPointerEventHandler, OnMouseButtonDown )
		SLATE_EVENT( FPointerEventHandler, OnMouseButtonUp )
		SLATE_EVENT( FPointerEventHandler, OnMouseMove )
		SLATE_EVENT( FPointerEventHandler, OnMouseDoubleClick )
		SLATE_EVENT( FPointerEventHandler, OnMouseDrag )
		SLATE_EVENT( FNoReplyPointerEventHandler, OnMouseHover )
		SLATE_EVENT( FSimpleNoReplyPointerEventHandler, OnMouseUnHover )
		SLATE_EVENT( FDragDropEventHandler, OnDragOver )
		SLATE_EVENT( FDragDropEventHandler, OnDrop )

	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

protected:
	virtual FReply OnDragDetected(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnDragOver(const FGeometry& MyGeometry, const FDragDropEvent& MouseEvent) override;
	virtual FReply OnDrop(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent) override;
	
private:
	FPointerEventHandler MouseDragHandler;
	FDragDropEventHandler DragOverHandler;
	FDragDropEventHandler DropHandler;
};
