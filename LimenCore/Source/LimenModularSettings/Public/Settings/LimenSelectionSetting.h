// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LimenSetting.h"
#include "LimenSelectionSetting.generated.h"


/**
 * Base class for a setting containing a selection (egg. resolution list)
 */
UCLASS(Abstract, Blueprintable, BlueprintType)
class LIMENMODULARSETTINGS_API ULimenSelectionSetting : public ULimenSetting, public TLimenEditableSetting<FString>, public TLimenReadableSetting<FString>
{
	GENERATED_BODY()

public:
	static const FString EnabledString; 
	static const FString DisabledString; 
	
	ULimenSelectionSetting();

	/// Readable Interface
	UFUNCTION(BlueprintCallable, Category="Limen|Modular Settings")
	virtual const TArray<FString>& GetSettingValues() const override final;
	UFUNCTION(BlueprintCallable, Category="Limen|Modular Settings")
	virtual FString GetCurrentValue() const override final;
	UFUNCTION(BlueprintCallable, Category="Limen|Modular Settings")
	virtual FString GetPreviousValue() const override final;
	UFUNCTION(BlueprintCallable, Category="Limen|Modular Settings")
	virtual FString GetAppliedValue() const override final;

	/// Editable Interface
protected:
	virtual bool IsValueValid(const FString& Test) override;
public:
	UFUNCTION(BlueprintCallable, Category="Limen|Modular Settings")	
	virtual bool CanEdit() const override;
	UFUNCTION(BlueprintCallable, Category="Limen|Modular Settings")	
	virtual bool SetNewValue(const FString& NewSelection) override;
	
	/// LimenSetting
protected:
	virtual void SetDefaults() override;
	virtual void PostDataLoaded() override final;
public:
	virtual void SetDefaultValue() override final;

	UFUNCTION(BlueprintCallable, Category="Limen|Modular Settings")
	bool IsEnabled() const;

protected:	
	UPROPERTY(EditAnywhere)
	TArray<FString> PossibleSelections;
	UPROPERTY(EditAnywhere)
	FString DefaultSelection;

	virtual void ApplyCurrentSetting(bool bUserRequest = false) override;
	
private:
	FString CurrentSelection;
	FString PreviousSelection;
	
	UPROPERTY(SaveGame)
	FString AppliedSelection;
};
