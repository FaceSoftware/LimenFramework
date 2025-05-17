// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LimenMenuButton.h"
#include "LimenSelectableMenuButton.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FButtonSelectedDelegate, ULimenSelectableMenuButton*, Button);

/**
 * 
 */
UCLASS()
class LIMENWIDGETS_API ULimenSelectableMenuButton : public ULimenMenuButton
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FButtonSelectedDelegate OnButtonSelected;
	UPROPERTY(BlueprintAssignable)
	FButtonSelectedDelegate OnButtonUnselected;

	ULimenSelectableMenuButton();

	UFUNCTION(BlueprintCallable)
	void SetButtonSelectedState(const bool bSelected);
	UFUNCTION(BlueprintCallable)
	bool IsButtonSelected() const;
	
protected:
	virtual FReply OnClicked() override;
	virtual void OnHovered() override;
	virtual void OnUnhovered() override;

private:
	bool bIsSelected;
};
