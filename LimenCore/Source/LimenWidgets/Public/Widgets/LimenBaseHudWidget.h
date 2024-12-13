// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LimenGameMenuWidget.h"
#include "LimenBaseHudWidget.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class LIMENWIDGETS_API ULimenBaseHudWidget : public ULimenGameMenuWidget
{
	GENERATED_BODY()

public:
	explicit ULimenBaseHudWidget(const FObjectInitializer& ObjectInitializer);
};
