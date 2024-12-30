// Copyright © 2024 FaceSoftware. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Settings/LimenSelectionSetting.h"
#include "LimenDisplayMonitorSetting.generated.h"

/**
 * 
 */
UCLASS()
class LIMENGRAPHICSETTINGS_API ULimenDisplayMonitorSetting : public ULimenSelectionSetting
{
	GENERATED_BODY()

public:
	ULimenDisplayMonitorSetting();

	virtual bool CanEdit() const override;
	
	virtual void ApplyCurrentSetting() override;
	virtual void SetDefaults() override;

private:
	
};
