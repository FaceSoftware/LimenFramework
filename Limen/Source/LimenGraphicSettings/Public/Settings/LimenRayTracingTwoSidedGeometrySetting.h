// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Settings/LimenToggleSetting.h"
#include "LimenRayTracingTwoSidedGeometrySetting.generated.h"

/**
 * 
 */
UCLASS()
class LIMENGRAPHICSETTINGS_API ULimenRayTracingTwoSidedGeometrySetting : public ULimenToggleSetting
{
	GENERATED_BODY()

public:
	ULimenRayTracingTwoSidedGeometrySetting();
	virtual bool CanEdit() const override;

protected:
	virtual void ApplyCurrentSetting(bool bUserRequest) override;
	virtual void SetDefaults() override;
};
