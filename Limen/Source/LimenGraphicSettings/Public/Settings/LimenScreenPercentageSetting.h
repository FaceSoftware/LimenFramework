// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Settings/LimenValueSetting.h"
#include "LimenScreenPercentageSetting.generated.h"

/**
 * 
 */
UCLASS()
class LIMENGRAPHICSETTINGS_API ULimenScreenPercentageSetting : public ULimenValueSetting
{
	GENERATED_BODY()

public:
	ULimenScreenPercentageSetting();
	
protected:
	virtual void ApplyCurrentSetting(bool bUserRequest) override;
	virtual void SetDefaults() override;
	
private:
	TConsoleSetting<float> Setting;
};
