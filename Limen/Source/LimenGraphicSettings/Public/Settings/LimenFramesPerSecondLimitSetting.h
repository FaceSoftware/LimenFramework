// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Settings/LimenValueSetting.h"
#include "LimenFramesPerSecondLimitSetting.generated.h"

/**
 * 
 */
UCLASS()
class LIMENGRAPHICSETTINGS_API ULimenFramesPerSecondLimitSetting : public ULimenValueSetting
{
	GENERATED_BODY()

public:
	ULimenFramesPerSecondLimitSetting();
	virtual void ApplyCurrentSetting(bool bUserRequest) override;
	
protected:
	virtual void SetDefaults() override;
	
};
