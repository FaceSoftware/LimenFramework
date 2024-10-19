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

	/// Readable Interface
	UFUNCTION(BlueprintCallable, Category="Limen|Modular Settings")
	virtual const TArray<float>& GetSettingValues() const override final;
	UFUNCTION(BlueprintCallable, Category="Limen|Modular Settings")
	virtual const float& GetCurrentValue() const override final;

	/// Editable Interface
	virtual bool IsValueValid(const float& Test) override;
	UFUNCTION(BlueprintCallable, Category="Limen|Modular Settings")	
	virtual bool CanEdit() const override;
	UFUNCTION(BlueprintCallable, Category="Limen|Modular Settings")	
	virtual bool SetNewValue(const float& NewSelection) override;
	
	/// LimenSetting
	virtual void SetDefaultValue() override final;

protected:
	/**
	 * @brief Use index 0 for minimum value and index 1 for the max value.
	 */
	UPROPERTY(EditAnywhere)
	TArray<float> ValueRange;
	UPROPERTY(EditAnywhere)
	float DefaultSettingValue;

	virtual void DataLoaded() override final;
	
private:	
	float CurrentSettingValue;

	// For serialization.
	// A float doesn't override it's value after loading if it's 0, don't know why...
	UPROPERTY(SaveGame)
	FString StringValue;
};
