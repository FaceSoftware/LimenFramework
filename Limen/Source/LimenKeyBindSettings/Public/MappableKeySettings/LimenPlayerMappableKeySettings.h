// Copyright © 2024 FaceSoftware. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "PlayerMappableKeySettings.h"
#include "LimenPlayerMappableKeySettings.generated.h"


UENUM(BlueprintType)
enum class EInputActionHandlingType : uint8
{
	NotApplicable,
	Hold,
	Toggle,
};

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class LIMENKEYBINDSETTINGS_API ULimenPlayerMappableKeySettings : public UPlayerMappableKeySettings
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	FText Description;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	bool bEditableInGame = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	bool bCanOverrideActionHandlingType = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	EInputActionHandlingType InputHandlingType = EInputActionHandlingType::NotApplicable;
};
