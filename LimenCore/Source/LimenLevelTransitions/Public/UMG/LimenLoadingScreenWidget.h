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
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
	virtual bool IsShowing() const override;
	virtual bool IsHiding() const override;
	
protected:
	UFUNCTION(BlueprintImplementableEvent)
	void PSOBatchingUpdated(const float CompletedPercentage, const int32 PSOsLeft);

private:
	/**
	 * @brief Overrides default method to keep widget persistent between level changes.
	 */
	virtual void ShowWidgetMethod() override;
	virtual void HideWidgetMethod() override;
	

	bool bIsShowing;
};
