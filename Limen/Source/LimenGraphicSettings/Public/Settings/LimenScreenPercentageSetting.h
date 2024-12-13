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
	virtual void ApplyCurrentSetting() override;
	virtual void SetDefaults() override;
	
private:
};
