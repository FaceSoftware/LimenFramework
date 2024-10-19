// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LimenWidget.h"
#include "LimenBaseHudWidget.generated.h"

/**
 * 
 */
UCLASS(Abstract, NotBlueprintable)
class LIMENWIDGETS_API ULimenBaseHudWidget : public ULimenWidget
{
	GENERATED_BODY()

public:
	ULimenBaseHudWidget(const FObjectInitializer& ObjectInitializer);
};
