// Copyright Face Software. All Rights Reserved.


#include "Settings/LimenNvidiaFrameGenerationSetting.h"

#include "StreamlineLibraryDLSSG.h"
#include "Settings/LimenFSRFrameGenerationSetting.h"
#include "Subsystems/LimenModularSettingsSubsystem.h"


ULimenNvidiaFrameGenerationSetting::ULimenNvidiaFrameGenerationSetting()
{
	DevelopmentName = TEXT("setting_nvidiaframegen");
	Category = FText::FromString(TEXT("Nvidia"));
	DisplayName = FText::FromString(TEXT("Nvidia DLSS Frame Generation"));
	Description = FText::FromString(TEXT("Nvidia DLSS frame generation mode."));
	bCanEdit = false;
}

void ULimenNvidiaFrameGenerationSetting::ApplyCurrentSetting()
{
	Super::ApplyCurrentSetting();

	if (const UStreamlineDLSSGMode Mode = UnFormatNvidiaFrameGenerationSetting(GetCurrentValue());
		UStreamlineLibraryDLSSG::IsDLSSGModeSupported(Mode))
	{
		IConsoleVariable* DLSSGConsoleVariable = IConsoleManager::Get().FindConsoleVariable(TEXT("r.Streamline.DLSSG.Enable"));
		DLSSGConsoleVariable->Set(Mode != UStreamlineDLSSGMode::Off, ECVF_SetByConsole);
		UStreamlineLibraryDLSSG::SetDLSSGMode(Mode);
	}
}

bool ULimenNvidiaFrameGenerationSetting::CanEdit() const
{
	if (!Super::CanEdit())
	{
		return false;
	}

	return UStreamlineLibraryDLSSG::IsDLSSGSupported() && bCanEdit;
}

void ULimenNvidiaFrameGenerationSetting::SubsystemInitialized(ULimenModularSettingsSubsystem* ModularSettingsSubsystem)
{
	Super::SubsystemInitialized(ModularSettingsSubsystem);

	ULimenFSRFrameGenerationSetting* AmdFsrUpscalingSetting = ModularSettingsSubsystem->GetItem<ULimenFSRFrameGenerationSetting>();
	if (AmdFsrUpscalingSetting != nullptr)
	{
		AmdFsrUpscalingSetting->OnSettingUpdated.AddUniqueDynamic(this, &ThisClass::OnFSRFrameGenerationSettingChanged);
	}
}

void ULimenNvidiaFrameGenerationSetting::SetDefaults()
{
	for (const auto& Mode : UStreamlineLibraryDLSSG::GetSupportedDLSSGModes())
	{
		PossibleSelections.Push(FormatNvidiaFrameGenerationSetting(Mode));
	}
	DefaultSelection = FormatNvidiaFrameGenerationSetting(UStreamlineDLSSGMode::Off);
}

FString ULimenNvidiaFrameGenerationSetting::FormatNvidiaFrameGenerationSetting(const UStreamlineDLSSGMode Mode)
{
	switch (Mode)
	{
	case UStreamlineDLSSGMode::On:
		return Enabled;
		
	case UStreamlineDLSSGMode::Off:
		return Disabled;
		
	default:
		break;
	}

	checkNoEntry();
	return {};
}

UStreamlineDLSSGMode ULimenNvidiaFrameGenerationSetting::UnFormatNvidiaFrameGenerationSetting(const FString& Selection)
{
	if (Selection.Contains(Enabled) == 0)
	{
		return UStreamlineDLSSGMode::On;
	}
	if (Selection.Contains(Disabled) == 0)
	{
		return UStreamlineDLSSGMode::Off;
	}

	checkNoEntry();
	return {};
}

void ULimenNvidiaFrameGenerationSetting::OnFSRFrameGenerationSettingChanged(const ULimenSetting* Setting)
{
	const ULimenFSRFrameGenerationSetting* FrameGenerationSetting = Cast<ULimenFSRFrameGenerationSetting>(Setting);
	if (FrameGenerationSetting->GetCurrentValue().Compare(ULimenFSRFrameGenerationSetting::Enabled) == 0 && bCanEdit)
	{
		// Other is enabled
		SetNewValue(Disabled);
		ApplyCurrentSetting();
		
		bCanEdit = false;
		OnSettingEditableStateChanged.Broadcast(this);
	}
	else if (FrameGenerationSetting->GetCurrentValue().Compare(ULimenFSRFrameGenerationSetting::Disabled) == 0 && !bCanEdit)
	{
		// Other is disabled
		bCanEdit = true;
		OnSettingEditableStateChanged.Broadcast(this);
	}
}
