// Copyright © 2024 FaceSoftware. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Settings/LimenSetting.h"
#include "LimenKeyBind.generated.h"

class UInputAction;

/**
 * 
 */
UCLASS()
class LIMENKEYBINDSETTINGS_API ULimenKeyBind : public ULimenSetting
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UInputAction> InputAction;	
};
