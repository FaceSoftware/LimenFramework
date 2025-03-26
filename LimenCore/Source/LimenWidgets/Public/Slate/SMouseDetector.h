// Copyright © 2024 FaceSoftware. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

/**
 * 
 */
class LIMENWIDGETS_API SMouseDetector : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SMouseDetector)
		: _OnMouseButtonDown()
		, _OnMouseButtonUp()
		, _OnMouseMove()
		, _OnMouseDoubleClick()
	{
	}
		
		SLATE_EVENT( FPointerEventHandler, OnMouseButtonDown )
		SLATE_EVENT( FPointerEventHandler, OnMouseButtonUp )
		SLATE_EVENT( FPointerEventHandler, OnMouseMove )
		SLATE_EVENT( FPointerEventHandler, OnMouseDoubleClick )
		SLATE_EVENT( FNoReplyPointerEventHandler, OnMouseHover )
		SLATE_EVENT( FSimpleNoReplyPointerEventHandler, OnMouseUnHover )

	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);
};
