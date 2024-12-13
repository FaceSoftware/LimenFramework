// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Settings/LimenSelectionSetting.h"
#include "LimenFramesPerSecondLimitSetting.generated.h"

/**
 * 
 */
UCLASS()
class LIMENGRAPHICSETTINGS_API ULimenFramesPerSecondLimitSetting : public ULimenSelectionSetting
{
	GENERATED_BODY()

public:
	ULimenFramesPerSecondLimitSetting();
	virtual void ApplyCurrentSetting() override;
	
protected:
	virtual void SetDefaults() override;
	
};
