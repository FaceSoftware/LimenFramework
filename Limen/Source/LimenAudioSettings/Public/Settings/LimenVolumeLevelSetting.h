// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Settings/LimenValueSetting.h"
#include "LimenVolumeLevelSetting.generated.h"

class USoundClass;

/**
 * 
 */
UCLASS(Blueprintable)
class LIMENAUDIOSETTINGS_API ULimenVolumeLevelSetting : public ULimenValueSetting
{
	GENERATED_BODY()

public:
	ULimenVolumeLevelSetting();
	virtual void ApplyCurrentSetting(bool bUserRequest) override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Limen")
	TSoftObjectPtr<USoundClass> SoundClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Limen")
	FName FMODParameterName;

	virtual void SetDefaults() override;
};
