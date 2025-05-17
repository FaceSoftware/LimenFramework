// Copyright FaceSoftware. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/Widget.h"
#include "Widgets/LimenWidget.h"
#include "LimenModularSettingsListWidget.generated.h"

namespace ETextJustify
{
	enum Type : int;
}

class ULimenSetting;
class ULimenToggleSetting;
class ULimenValueSetting;
class ULimenSelectionSetting;
class ULimenModularSettingsSubsystem;


UCLASS(Abstract, NotBlueprintable)
class ULimenSettingWidget : public ULimenWidget
{
	GENERATED_BODY()

public:
	virtual void BindSetting(ULimenSetting* InSetting) {}
};

UCLASS(Blueprintable)
class ULimenValueSettingWidget : public ULimenSettingWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void BindSetting(ULimenSetting* InSetting) override;

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void SettingBound(ULimenValueSetting* Setting);

private:
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	TWeakObjectPtr<ULimenValueSetting> BoundSetting;
};


UCLASS(Blueprintable)
class ULimenSelectionSettingWidget : public ULimenSettingWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void BindSetting(ULimenSetting* InSetting) override;

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void SettingBound(ULimenSelectionSetting* Setting);

private:
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	TWeakObjectPtr<ULimenSelectionSetting> BoundSetting;
};


UCLASS(Blueprintable)
class ULimenToggleSettingWidget : public ULimenSettingWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void BindSetting(ULimenSetting* InSetting) override;

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void SettingBound(ULimenToggleSetting* Setting);

private:
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	TWeakObjectPtr<ULimenToggleSetting> BoundSetting;
};



UCLASS(Blueprintable, BlueprintType)
class LIMENMODULARSETTINGS_API ULimenSettingsListWidget : public UWidget
{
	GENERATED_BODY()

public:
	ULimenSettingsListWidget();

protected:
	UPROPERTY(EditAnywhere, Category="Settings Subsystem")
	TSubclassOf<ULimenModularSettingsSubsystem> ModularSettingsSubsystem;
	UPROPERTY(EditAnywhere, Category="Settings")
	TSubclassOf<ULimenValueSettingWidget> ValueSettingWidgetClass;
	UPROPERTY(EditAnywhere, Category="Settings")
	TSubclassOf<ULimenSelectionSettingWidget> SelectionSettingWidgetClass;
	UPROPERTY(EditAnywhere, Category="Settings")
	TSubclassOf<ULimenToggleSettingWidget> ToggleSettingWidgetClass;
	UPROPERTY(EditAnywhere, Category="Settings")
	TEnumAsByte<EVerticalAlignment> SettingVerticalAlignment;
	UPROPERTY(EditAnywhere, Category="Settings")
	TEnumAsByte<EHorizontalAlignment> SettingHorizontalAlignment;
	UPROPERTY(EditAnywhere, Category="Categories")
	TEnumAsByte<EVerticalAlignment> CategoryVerticalAlignment;
	UPROPERTY(EditAnywhere, Category="Categories")
	TEnumAsByte<EHorizontalAlignment> CategoryHorizontalAlignment;
	UPROPERTY(EditAnywhere, Category="Categories")
	FSlateColor CategoryTextColor;
	UPROPERTY(EditAnywhere, Category="Categories")
	FSlateFontInfo CategoryFont;
	UPROPERTY(EditAnywhere, Category="Categories")
	TEnumAsByte<ETextJustify::Type> CategoryTextJustification;
	UPROPERTY(EditAnywhere, Category="Categories")
	FMargin CategoryTextMargin;

	virtual TSharedRef<SWidget> RebuildWidget() override;

	virtual bool AreParamsValid() const;
	virtual void ResolveSubsystem();

private:
	UPROPERTY()
	TWeakObjectPtr<ULimenModularSettingsSubsystem> Subsystem;

};
