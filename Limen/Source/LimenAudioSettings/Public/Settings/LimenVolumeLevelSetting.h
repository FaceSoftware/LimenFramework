// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Settings/LimenValueSetting.h"
#include "LimenVolumeLevelSetting.generated.h"

namespace FMOD::Studio
{
	class Bus;
}

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
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Volume Level Setting")
	TObjectPtr<USoundClass> SoundClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Volume Level Setting")
	TObjectPtr<USoundMix> SoundMix;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Volume Level Setting|FMOD")
	FString FmodBusName;

	virtual void SetDefaults() override;

private:
	FMOD::Studio::Bus* FmodBus;
};
