// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Settings/LimenValueSetting.h"
#include "LimenSensitivitySetting.generated.h"


class ALimenPlayerCharacter;
class ULimenRecurrentAction;
/**
 * 
 */
UCLASS()
class LIMENGAMESETTINGS_API ULimenSensitivitySetting : public ULimenValueSetting
{
	GENERATED_BODY()

public:
	ULimenSensitivitySetting();

protected:
	virtual void SetDefaults() override;

	virtual void RecurrentAction() override;
	virtual bool ShouldStopRecurrentAction() override;
	virtual void ActionSuccessful() override;

private:
	UPROPERTY()
	TObjectPtr<ALimenPlayerCharacter> PlayerCharacter;
};
