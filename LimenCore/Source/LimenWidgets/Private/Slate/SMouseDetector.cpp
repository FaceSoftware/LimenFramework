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

	ChildSlot
	.HAlign(EHorizontalAlignment::HAlign_Fill)
	.VAlign(EVerticalAlignment::VAlign_Fill)
	.Padding(FMargin(0.f));
}
