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
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Limen")
	FString FmodVcaName;

	FMOD::Studio::Bus* FmodBus;

	virtual void SetDefaults() override;
};
