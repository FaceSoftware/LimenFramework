// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Templates/SubclassOf.h"
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

	void SetHintWidgetClass(const TSubclassOf<ULimenHintWidget>& InClass);
	
	void Initialize();
	void Enable();
	void Disable();

	bool HasEverBeenVisible() const;

	ULimenHintWidget* GetHintWidget() const;
	
protected:
	virtual void SetHintText(FText& OutTitle, FText& OutBody);

	virtual void PreSetup();
	virtual void SetupShowHint();
	virtual void SetupHideHint();

	void ShowHint();
	void HideHint();
	
private:
	UPROPERTY()
	TObjectPtr<ULimenHintWidget> HintWidgetInstance;
	UPROPERTY()
	TSubclassOf<ULimenHintWidget> HintWidgetClass;

	bool bIsEnabled;
	bool bHasTriedToShowHint;
};
