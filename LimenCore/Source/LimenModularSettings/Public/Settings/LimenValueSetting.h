// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LimenSetting.h"
#include "LimenValueSetting.generated.h"

/**
 * Base class for a ranged value setting (egg. volume usually is set from 0 to 100)
 */
UCLASS(Abstract, Blueprintable, BlueprintType)
class LIMENMODULARSETTINGS_API ULimenValueSetting : public ULimenSetting, public TLimenEditableSetting<float>, public TLimenReadableSetting<float>
{
	GENERATED_BODY()

public:
	ULimenValueSetting();

	UFUNCTION(BlueprintCallable)
	float GetMinValuePerChange() const;
	UFUNCTION(BlueprintCallable)
	int32 GetDecimalCount() const;

	/// Readable Interface
	UFUNCTION(BlueprintCallable, Category="Limen|Modular Settings")
	virtual const TArray<float>& GetSettingValues() const override final;
	UFUNCTION(BlueprintCallable, Category="Limen|Modular Settings")
	virtual float GetCurrentValue() const override final;
	UFUNCTION(BlueprintCallable, Category="Limen|Modular Settings")
	virtual float GetPreviousValue() const override final;

	/// Editable Interface
	virtual bool IsValueValid(const float& Test) override;
	UFUNCTION(BlueprintCallable, Category="Limen|Modular Settings")	
	virtual bool CanEdit() const override;
	UFUNCTION(BlueprintCallable, Category="Limen|Modular Settings")	
	virtual bool SetNewValue(const float& NewSelection) override;
	
	/// LimenSetting
	virtual void SetDefaults() override;
	virtual void SetDefaultValue() override final;
	virtual void PostDataLoaded() override final;

protected:
	/**
	 * @brief Use index 0 for minimum value and index 1 for the max value.
	 */
	UPROPERTY(EditAnywhere)
	TArray<float> ValueRange;
	UPROPERTY(EditAnywhere)
	float DefaultSettingValue;

	UPROPERTY(EditAnywhere, meta=(ClampMin="0"))
	int32 DecimalsDisplayed;
	
private:	
	float CurrentSettingValue;
	float PreviousSettingValue;

	// For serialization.
	// A float doesn't override its value after loading if it's 0, don't know why...
	UPROPERTY(SaveGame)
	FString StringValue;

	float MinValuePerChange;
};
