// Copyright © 2024 FaceSoftware. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BlueprintAsyncActions/LimenTickCheck.h"
#include "Settings/LimenSetting.h"
#include "LimenBrightnessSetting.generated.h"


class APostProcessVolume;
struct FLimenPolymorphicData;

USTRUCT(BlueprintType)
struct FBrightnessSettingValues
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, SaveGame)
	float HighToneValue = 1.f;
	UPROPERTY(BlueprintReadOnly)
	FVector2D HighToneLimits = FVector2D(0.5f, 2.f);

	UPROPERTY(BlueprintReadWrite, SaveGame)
	float LowToneValue = 1.f;
	UPROPERTY(BlueprintReadOnly)
	FVector2D LowToneLimits = FVector2D(0.f, 1.5f);
};

/**
 * 
 */
UCLASS()
class LIMENGRAPHICSETTINGS_API ULimenBrightnessSetting : public ULimenSetting, public TLimenEditableSetting<FBrightnessSettingValues>, public TLimenReadableSetting<FBrightnessSettingValues>
{
	GENERATED_BODY()

public:
	ULimenBrightnessSetting();

	/// Readable Interface
	UFUNCTION(BlueprintCallable, Category="Limen|Modular Settings")
	virtual const TArray<FBrightnessSettingValues>& GetSettingValues() const override final;
	UFUNCTION(BlueprintCallable, Category="Limen|Modular Settings")
	virtual FBrightnessSettingValues GetCurrentValue() const override final;
	UFUNCTION(BlueprintCallable, Category="Limen|Modular Settings")
	virtual FBrightnessSettingValues GetPreviousValue() const override final;

	/// Editable Interface
	virtual bool IsValueValid(const FBrightnessSettingValues& Test) override;
	UFUNCTION(BlueprintCallable, Category="Limen|Modular Settings")	
	virtual bool CanEdit() const override;
	UFUNCTION(BlueprintCallable, Category="Limen|Modular Settings")	
	virtual bool SetNewValue(const FBrightnessSettingValues& NewSelection) override;
	
	/// LimenSetting
	virtual void ApplyCurrentSetting(bool bUserRequest = false) override;
	virtual void SetDefaults() override;
	virtual void SetDefaultValue() override final;
	virtual void DataLoaded() override final;

private:
	UPROPERTY(SaveGame)
	FBrightnessSettingValues CurrentSelection;
	FBrightnessSettingValues PreviousSelection;

	FBrightnessSettingValues DefaultSelection;
	TArray<FBrightnessSettingValues> DummyData;

	void GlobalPostProcessFound(APostProcessVolume* InGlobalPostProcess) const;
};
