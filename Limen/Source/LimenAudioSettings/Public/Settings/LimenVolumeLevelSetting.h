// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Settings/LimenValueSetting.h"
#include "LimenVolumeLevelSetting.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class LIMENAUDIOSETTINGS_API ULimenVolumeLevelSetting : public ULimenValueSetting
{
	GENERATED_BODY()

public:
	ULimenVolumeLevelSetting();
	virtual void ApplyCurrentSetting() override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Limen")
	TSoftObjectPtr<USoundClass> SoundClass;

	virtual void SetDefaults() override;
};
