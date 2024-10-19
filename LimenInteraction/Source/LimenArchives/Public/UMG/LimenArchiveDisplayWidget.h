// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/LimenWidget.h"
#include "LimenArchiveDisplayWidget.generated.h"

class UWidgetSwitcher;
class ULimenArchive;
/**
 * 
 */
UCLASS()
class LIMENARCHIVES_API ULimenArchiveDisplayWidget : public ULimenWidget
{
	GENERATED_BODY()

public:
	
protected:
	UPROPERTY(BlueprintReadOnly, meta=(ExposeOnSpawn))
	TObjectPtr<ULimenArchive> Archive;

private:
};
