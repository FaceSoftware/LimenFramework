// Copyright © 2024 FaceSoftware. All rights reserved.


#include "Slate/SMouseDetector.h"

#include "SlateOptMacros.h"


void SMouseDetector::Construct(const FArguments& InArgs)
{
	if (InArgs._OnMouseButtonDown.IsBound())
	{
		SetOnMouseButtonDown(InArgs._OnMouseButtonDown);
	}

	if (InArgs._OnMouseButtonUp.IsBound())
	{
		SetOnMouseButtonUp(InArgs._OnMouseButtonUp);
	}

	if (InArgs._OnMouseMove.IsBound())
	{
		SetOnMouseMove(InArgs._OnMouseMove);
	}

	if (InArgs._OnMouseDoubleClick.IsBound())
	{
		SetOnMouseDoubleClick(InArgs._OnMouseDoubleClick);
	}

	if (InArgs._OnMouseHover.IsBound())
	{
		SetOnMouseEnter(InArgs._OnMouseHover);
	}

	if (InArgs._OnMouseUnHover.IsBound())
	{
		SetOnMouseLeave(InArgs._OnMouseUnHover);
	}

	if (InArgs._OnMouseDrag.IsBound())
	{
		MouseDragHandler = InArgs._OnMouseDrag;
	}

	ChildSlot
	.HAlign(HAlign_Fill)
	.VAlign(VAlign_Fill)
	.Padding(FMargin(0.f));
}

FReply SMouseDetector::OnDragDetected(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (MouseDragHandler.IsBound())
	{
		return MouseDragHandler.Execute(MyGeometry, MouseEvent);
	}

	return FReply::Unhandled();
}
