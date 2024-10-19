// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LimenWidgets/Public/Widgets/LimenWidget.h"
#include "LimenHintWidget.generated.h"

/**
 * 
 */
UCLASS()
class LIMENHINTS_API ULimenHintWidget : public ULimenWidget
{
	GENERATED_BODY()

public:
	void SetHintText(const FText& InTitle, const FText& InText);

protected:
	UPROPERTY(BlueprintReadOnly)
	FText Title;
	UPROPERTY(BlueprintReadOnly)
	FText Text;
	
};
