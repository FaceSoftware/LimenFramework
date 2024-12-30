// Copyright Face Software. All Rights Reserved.


#pragma once

#include "CoreMinimal.h"
#include "Settings/LimenSetting.h"
#include "LimenToggleSetting.generated.h"

/**
 * Base class for a setting that can be toggled (on or off)
 */
UCLASS(Abstract, Blueprintable, BlueprintType)
class LIMENMODULARSETTINGS_API ULimenToggleSetting : public ULimenSetting, public TLimenEditableSetting<bool>, public TLimenReadableSetting<bool>
{
	GENERATED_BODY()

public:
	
	ULimenToggleSetting();

	/// Readable Interface
	UFUNCTION(BlueprintCallable, Category="Limen|Modular Settings")
	virtual const TArray<bool>& GetSettingValues() const override final;
	UFUNCTION(BlueprintCallable, Category="Limen|Modular Settings")
	virtual bool GetCurrentValue() const override final;
	UFUNCTION(BlueprintCallable, Category="Limen|Modular Settings")
	virtual bool GetPreviousValue() const override final;

	/// Editable Interface
	virtual bool IsValueValid(const bool& Test) override;
	UFUNCTION(BlueprintCallable, Category="Limen|Modular Settings")	
	virtual bool CanEdit() const override;
	UFUNCTION(BlueprintCallable, Category="Limen|Modular Settings")	
	virtual bool SetNewValue(const bool& NewSelection) override;

	/// LimenSetting
	virtual void SetDefaults() override;
	virtual void SetDefaultValue() override final;
	virtual void DataLoaded() override final;
	
protected:
	UPROPERTY(EditAnywhere)
	bool bDefaultSettingState;
	TArray<bool> PossibleStates;
	
private:
	UPROPERTY(SaveGame)
	bool bCurrentSettingState;
	bool bPreviousSettingState;
};
