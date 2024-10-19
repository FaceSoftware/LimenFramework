// Copyright Face Software. All Rights Reserved.


#include "Settings/LimenFSRUpscalingSetting.h"

#include "Settings/LimenNvidiaImageScalingSetting.h"
#include "Subsystems/LimenModularSettingsSubsystem.h"


ULimenFSRUpscalingSetting::ULimenFSRUpscalingSetting()
{
	DevelopmentName = TEXT("setting_fsrquality");
	Category = FText::FromString(TEXT("AMD"));
	DisplayName = FText::FromString(TEXT("FidelityFX FSR3 Upscaling"));
	Description = FText::FromString(TEXT("FidelityFX FSR3 provides a similar visual quality at a lower cost in performance. Please note that only one upscaler can be on at a time (Nvidia NIS or FSR3)."));
	bCanEdit = true;
}

bool ULimenFSRUpscalingSetting::CanEdit() const
{
	if (!Super::CanEdit())
	{
		return false;
	}
	
	return bCanEdit;
}

void ULimenFSRUpscalingSetting::SubsystemInitialized(ULimenModularSettingsSubsystem* ModularSettingsSubsystem)
{
	Super::SubsystemInitialized(ModularSettingsSubsystem);

	ULimenNvidiaImageScalingSetting* AmdFsrUpscalingSetting = ModularSettingsSubsystem->GetItem<ULimenNvidiaImageScalingSetting>();
	AmdFsrUpscalingSetting->OnSettingUpdated.AddUniqueDynamic(this, &ThisClass::OnNisSettingChanged);
}

void ULimenFSRUpscalingSetting::SetDefaults()
{
	Super::SetDefaults();

	PossibleSelections.Reserve(6);
	PossibleSelections.Push(Disabled);
	PossibleSelections.Push(Quality);
	PossibleSelections.Push(Balanced);
	PossibleSelections.Push(Performance);
	PossibleSelections.Push(UltraPerformance);
	DefaultSelection = Disabled;
}

void ULimenFSRUpscalingSetting::ApplyCurrentSetting()
{
	Super::ApplyCurrentSetting();

	const uint8 SettingValue = UnFormatFsrSetting(GetCurrentValue());
	
	IConsoleVariable* FsrQualityModeVariable = IConsoleManager::Get().FindConsoleVariable(TEXT("r.FidelityFX.FSR3.QualityMode"));
	FsrQualityModeVariable->Set(SettingValue, ECVF_SetByConsole);

	IConsoleVariable* FsrEnableConsoleVariable = IConsoleManager::Get().FindConsoleVariable(TEXT("r.FidelityFX.FSR3.Enabled"));
	FsrEnableConsoleVariable->Set(SettingValue != 5);

	if (SettingValue == 5)
	{
		IConsoleManager::Get().FindConsoleVariable(TEXT("r.ScreenPercentage"))->Set(100.f);
	}
	
}

FString ULimenFSRUpscalingSetting::FormatFsrSetting(const uint8 InValue)
{
	switch (InValue)
	{
	case 1:
		return Quality;

	case 2:
		return Balanced;

	case 3:
		return Performance;

	case 4:
		return UltraPerformance;
		
	case 5:
		return Disabled;
		
	default:
		break;
	}
	
	checkNoEntry();
	return {};
}

uint8 ULimenFSRUpscalingSetting::UnFormatFsrSetting(const FString& InValue)
{
	uint8 NewValue;
	if (InValue.Compare(Quality) == 0)
	{
		NewValue = 1;
	}
	else if (InValue.Compare(Balanced) == 0)
	{
		NewValue = 2;
	}
	else if (InValue.Compare(Performance) == 0)
	{
		NewValue = 3;
	}
	else if (InValue.Compare(UltraPerformance) == 0)
	{
		NewValue = 4;
	}
	else if (InValue.Compare(Disabled) == 0)
	{
		NewValue = 5;
	}
	else
	{
		checkNoEntry();
		NewValue = {};
	}
	
	return NewValue;
}

void ULimenFSRUpscalingSetting::OnNisSettingChanged(const ULimenSetting* Setting)
{
	const ULimenNvidiaImageScalingSetting* NisSetting = Cast<ULimenNvidiaImageScalingSetting>(Setting);
	if (NisSetting->GetCurrentValue() != ULimenNvidiaImageScalingSetting::Disabled && bCanEdit)
	{
		SetNewValue(Disabled);
		ApplyCurrentSetting();

		bCanEdit = false;
		OnSettingEditableStateChanged.Broadcast(this);
	}
	else if (NisSetting->GetCurrentValue() == ULimenNvidiaImageScalingSetting::Disabled && !bCanEdit)
	{
		bCanEdit = true;
		OnSettingEditableStateChanged.Broadcast(this);
	}
}
