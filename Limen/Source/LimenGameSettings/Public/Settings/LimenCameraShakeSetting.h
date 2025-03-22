// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Settings/LimenSelectionSetting.h"
#include "LimenCameraShakeSetting.generated.h"

/**
 * 
 */
UCLASS()
class LIMENGAMESETTINGS_API ULimenCameraShakeSetting : public ULimenSelectionSetting
{
	GENERATED_BODY()

public:	
	ULimenCameraShakeSetting();
	
protected:
	virtual void SetDefaults() override;
	virtual void ApplyCurrentSetting(bool bUserRequest) override;

private:
	FDelegateHandle ApplySettingToSpawnedActorHandle;
	
	static FString FormatCameraShakeSetting(const bool bEnabled);
	static bool UnFormatCameraShakeSetting(const FString& Selection);

	void ApplyCurrentSettingInternal(AActor* Actor) const;
};
