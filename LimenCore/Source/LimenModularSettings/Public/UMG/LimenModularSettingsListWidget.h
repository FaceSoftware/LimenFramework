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


UCLASS(Abstract, Blueprintable)
class LIMENMODULARSETTINGS_API ULimenSettingWidget : public ULimenWidget
{
	GENERATED_BODY()

public:
	explicit ULimenSettingWidget(const FObjectInitializer& ObjectInitializer);
	virtual void NativeConstruct() override;
	virtual void BindSetting(ULimenSetting* InSetting);
	template<typename T>
	T* GetBoundSetting()
	{
		static_assert(TIsDerivedFrom<T, ULimenSetting>::Value, "T must be derived from ULimenSetting");
		return CastChecked<T>(SettingPtr.Get());
	}

protected:
	UFUNCTION(BlueprintNativeEvent)
	void SettingUpdated(const ULimenSetting* Setting);
	virtual void SettingUpdated_Implementation(const ULimenSetting* Setting) {}
	UFUNCTION(BlueprintNativeEvent)
	void SettingApplied(const ULimenSetting* Setting);
	virtual void SettingApplied_Implementation(const ULimenSetting* Setting) {}
	UFUNCTION(BlueprintNativeEvent)
	void SettingEditableStateChanged(const ULimenSetting* Setting);
	virtual void SettingEditableStateChanged_Implementation(const ULimenSetting* Setting) {}
	UFUNCTION(BlueprintNativeEvent)
	void SettingBound(const ULimenSetting* Setting);
	virtual void SettingBound_Implementation(const ULimenSetting* Setting) {}

private:
	TWeakObjectPtr<ULimenSetting> SettingPtr;
};

UCLASS(Blueprintable)
class LIMENMODULARSETTINGS_API ULimenValueSettingWidget : public ULimenSettingWidget
{
	GENERATED_BODY()

public:
	virtual void BindSetting(ULimenSetting* InSetting) override;
	FORCEINLINE ULimenValueSetting* GetValueSetting() const { return BoundSetting.Get(); }
	

protected:

private:
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	TWeakObjectPtr<ULimenValueSetting> BoundSetting;
};


UCLASS(Blueprintable)
class LIMENMODULARSETTINGS_API ULimenSelectionSettingWidget : public ULimenSettingWidget
{
	GENERATED_BODY()

public:
	virtual void BindSetting(ULimenSetting* InSetting) override;
	FORCEINLINE ULimenSelectionSetting* GetSelectionSetting() const { return BoundSetting.Get(); }

protected:

private:
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	TWeakObjectPtr<ULimenSelectionSetting> BoundSetting;
};


UCLASS(Blueprintable)
class LIMENMODULARSETTINGS_API ULimenToggleSettingWidget : public ULimenSettingWidget
{
	GENERATED_BODY()

public:
	virtual void BindSetting(ULimenSetting* InSetting) override;
	FORCEINLINE ULimenToggleSetting* GetToggleSetting() const { return BoundSetting.Get(); }

protected:

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
	UPROPERTY(EditAnywhere, Category="Settings Display", meta=(AllowAbstract="true"))
	TMap<TSubclassOf<ULimenSetting>, TSubclassOf<ULimenSettingWidget>> SettingWidgetClasses;
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

	TSubclassOf<ULimenSettingWidget> GetSettingWidget(const ULimenSetting* Setting);
};
