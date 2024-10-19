// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Settings/LimenSelectionSetting.h"
#include "LimenNvidiaReflexSetting.generated.h"

enum class UStreamlineReflexMode : uint8;
/**
 * 
 */
UCLASS()
class LIMENGRAPHICSETTINGS_API ULimenNvidiaReflexSetting : public ULimenSelectionSetting
{
	GENERATED_BODY()
	

public:
	ULimenNvidiaReflexSetting();
	virtual bool CanEdit() const override;

protected:
	virtual void ApplyCurrentSetting() override;
	virtual void SetDefaults() override;

private:
	inline static const FString Disabled = TEXT("Disabled");
	inline static const FString Enabled = TEXT("Enabled");
	inline static const FString EnabledPlusBoost = TEXT("Enabled + Boost");
	
	static FString FormatNvidiaReflexSetting(const UStreamlineReflexMode Mode);
	static UStreamlineReflexMode UnFormatNvidiaReflexSetting(const FString& Selection);
};
