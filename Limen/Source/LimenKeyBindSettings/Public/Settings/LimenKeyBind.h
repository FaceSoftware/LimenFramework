// Copyright © 2024 FaceSoftware. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedActionKeyMapping.h"
#include "Settings/LimenSetting.h"
#include "LimenKeyBind.generated.h"


enum class EInputActionHandlingType : uint8;
class ULimenPlayerMappableKeySettings;
class UInputAction;

USTRUCT()
struct FSaveableEnhancedActionKeyMapping : public FEnhancedActionKeyMapping
{
	GENERATED_BODY()

	FSaveableEnhancedActionKeyMapping() = default;
	explicit FSaveableEnhancedActionKeyMapping(const FEnhancedActionKeyMapping& InKeyMapping);
    bool Serialize(FArchive& Ar);
};

/**
 * 
 */
UCLASS(Blueprintable)
class LIMENKEYBINDSETTINGS_API ULimenKeyBind : public ULimenSetting, public TLimenEditableSetting<FEnhancedActionKeyMapping>, public TLimenReadableSetting<FEnhancedActionKeyMapping>
{
	GENERATED_BODY()

public:		
	ULimenKeyBind();

	virtual void Serialize(FArchive& Ar) override;
	void InitializeSetting(ULimenModularSettingsSubsystem* InOwnerSubsystem, FEnhancedActionKeyMapping* InActionKeyMapping);
	
	/// Readable Interface
	UFUNCTION(BlueprintCallable, Category="Limen|Modular Settings")
	virtual const TArray<FEnhancedActionKeyMapping>& GetSettingValues() const override final;
	UFUNCTION(BlueprintCallable, Category="Limen|Modular Settings")
	virtual FEnhancedActionKeyMapping GetCurrentValue() const override final;
	UFUNCTION(BlueprintCallable, Category="Limen|Modular Settings")
	virtual FEnhancedActionKeyMapping GetPreviousValue() const override final;

	/// Editable Interface
	virtual bool IsValueValid(const FEnhancedActionKeyMapping& Test) override;
	UFUNCTION(BlueprintCallable, Category="Limen|Modular Settings")	
	virtual bool CanEdit() const override;
	UFUNCTION(BlueprintCallable, Category="Limen|Modular Settings")	
	virtual bool SetNewValue(const FEnhancedActionKeyMapping& NewSelection) override;
	
	/// LimenSetting
	virtual void ApplyCurrentSetting(bool bUserRequest = false) override;
	virtual void SetDefaults() override;
	virtual void SetDefaultValue() override final;
	virtual void DataLoaded() override final;

	UFUNCTION(BlueprintCallable)
	void SetInputActionHandlingType(const EInputActionHandlingType NewType);

protected:
	UPROPERTY(EditAnywhere)
	bool bCanEdit;

private:
	UPROPERTY(SaveGame)
	FSaveableEnhancedActionKeyMapping CurrentKeyMapping;
	FEnhancedActionKeyMapping PreviousKeyMapping;
	TArray<FEnhancedActionKeyMapping> PossibleValues;
	FEnhancedActionKeyMapping DefaultSelection;
	
	FEnhancedActionKeyMapping* ActionKeyMappingPtr;
};