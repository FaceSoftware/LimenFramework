// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "LimenHint.generated.h"

class ULimenHintWidget;
class ALimenHintsManager;

DECLARE_MULTICAST_DELEGATE(FHintDelegate)

/**
 * 
 */
UCLASS(Abstract, Blueprintable, BlueprintType)
class LIMENHINTS_API ULimenHint : public UObject
{
	GENERATED_BODY()

public:
	FHintDelegate OnHintDismissed;

	ULimenHint();
	
	void Initialize();
	void Enable();
	void Disable();
	
protected:
	virtual void SetHintText(FText& OutTitle, FText& OutBody);
	virtual void SetupShowHint();
	virtual void SetupHideHint();

	void ShowHint();
	void HideHint();
	
private:
	UPROPERTY()
	TObjectPtr<ULimenHintWidget> HintWidgetInstance;

	bool bIsEnabled;
	bool bHasTriedToShowHint;
};
