// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/LimenBaseHudWidget.h"
#include "LimenLoadingScreenWidget.generated.h"

class ALimenBaseHUD;
/**
 * 
 */
UCLASS()
class LIMENLEVELTRANSITIONS_API ULimenLoadingScreenWidget : public ULimenWidget
{
	GENERATED_BODY()

public:
	explicit ULimenLoadingScreenWidget(const FObjectInitializer& ObjectInitializer);
	
protected:

private:
	/**
	 * @brief Overrides default method to keep widget persistent between level changes.
	 */
	virtual void ShowWidgetMethod() override;
};
