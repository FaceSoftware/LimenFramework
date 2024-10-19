// Copyright Face Software. All Rights Reserved.


#include "Settings/LimenNvidiaImageScalingSetting.h"

#include "NISLibrary.h"
#include "Settings/LimenFSRUpscalingSetting.h"
#include "Subsystems/LimenGraphicalSettingsSubsystem.h"


ULimenNvidiaImageScalingSetting::ULimenNvidiaImageScalingSetting()
{
	DevelopmentName = TEXT("setting_nis");
	Category = FText::FromString(TEXT("Nvidia"));
	DisplayName = FText::FromString(TEXT("Nvidia DLSS Image Scaling"));
	Description = FText::FromString(TEXT("Sets the Nvidia DLSS Image Scaling (NIS) mode."));
	bCanEdit = true;
}

bool ULimenNvidiaImageScalingSetting::CanEdit() const
{
	if (!Super::CanEdit())
	{
		return false;
	}

	return UNISLibrary::IsNISSupported() && bCanEdit;
}

void ULimenNvidiaImageScalingSetting::SubsystemInitialized(ULimenModularSettingsSubsystem* ModularSettingsSubsystem)
{
	Super::SubsystemInitialized(ModularSettingsSubsystem);
	
	ULimenFSRUpscalingSetting* AmdFsrUpscalingSetting = ModularSettingsSubsystem->GetItem<ULimenFSRUpscalingSetting>();
	if (AmdFsrUpscalingSetting != nullptr)
	{
		AmdFsrUpscalingSetting->OnSettingUpdated.AddUniqueDynamic(this, &ThisClass::OnFsrModeChanged);
	}
}

void ULimenNvidiaImageScalingSetting::ApplyCurrentSetting()
{
	Super::ApplyCurrentSetting();

	const UNISMode Mode = UnFormatNisSetting(GetCurrentValue());
	if (UNISLibrary::IsNISModeSupported(Mode))
	{
		IConsoleVariable* NISConsoleVariable = IConsoleManager::Get().FindConsoleVariable(TEXT("r.NIS.Enable"));
		NISConsoleVariable->Set(GetCurrentValue() != FormatNisSetting(UNISMode::Off), ECVF_SetByConsole);
		UNISLibrary::SetNISMode(Mode);
		
		if (Mode == UNISMode::Off)
		{
			IConsoleManager::Get().FindConsoleVariable(TEXT("r.ScreenPercentage"))->Set(100.f);
		}
	}
}

void ULimenNvidiaImageScalingSetting::SetDefaults()
{
	for (const auto& Mode : UNISLibrary::GetSupportedNISModes())
	{
		if (Mode == UNISMode::Custom)
		{
			continue;
		}

		PossibleSelections.Push(FormatNisSetting(Mode));
	}
	DefaultSelection = FormatNisSetting(UNISMode::Off);
}

FString ULimenNvidiaImageScalingSetting::FormatNisSetting(const UNISMode Mode)
{
	switch (Mode)
	{
	case UNISMode::Off:
		return Disabled;
		
	case UNISMode::UltraQuality:
		return UltraQuality;
		
	case UNISMode::Quality:
		return Quality;
		
	case UNISMode::Balanced:
		return Balanced;
		
	case UNISMode::Performance:
		return Performance;
		
	default:
		checkNoEntry();
		break;
	}

	return {};
}

UNISMode ULimenNvidiaImageScalingSetting::UnFormatNisSetting(const FString& Selection)
{
	if (Selection.Compare(Disabled) == 0)
	{
		return UNISMode::Off;
	}
	if (Selection.Compare(UltraQuality) == 0)
	{
		return UNISMode::UltraQuality;
	}
	if (Selection.Compare(Quality) == 0)
	{
		return UNISMode::Quality;
	}
	if (Selection.Compare(Balanced) == 0)
	{
		return UNISMode::Balanced;
	}
	if (Selection.Compare(Performance) == 0)
	{
		return UNISMode::Performance;
	}

	checkNoEntry();
	return UNISMode::Off;
}

void ULimenNvidiaImageScalingSetting::OnFsrModeChanged(const ULimenSetting* SettingChanged)
{
	const ULimenFSRUpscalingSetting* Setting = Cast<ULimenFSRUpscalingSetting>(SettingChanged);
	if (Setting->GetCurrentValue().Compare(ULimenFSRUpscalingSetting::Disabled) != 0 && bCanEdit)
	{
		// Other is enabled
		SetNewValue(Disabled);
		ApplyCurrentSetting();
		
		bCanEdit = false;
		OnSettingEditableStateChanged.Broadcast(this);
	}
	else if (Setting->GetCurrentValue().Compare(ULimenFSRUpscalingSetting::Disabled) == 0 && !bCanEdit)
	{
		// Other is disabled
		bCanEdit = true;
		OnSettingEditableStateChanged.Broadcast(this);
	}
	
}
