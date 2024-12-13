// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/LimenMenuWidget.h"
#include "LimenPauseMenuWidget.generated.h"


UCLASS(Abstract, Blueprintable)
class LIMENPLAYERS_API ULimenPauseMenuWidget : public ULimenMenuWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void UnpauseGame();
};
