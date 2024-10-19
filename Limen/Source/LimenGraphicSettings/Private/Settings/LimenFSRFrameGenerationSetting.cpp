// Copyright Face Software. All Rights Reserved.


#include "Settings/LimenFSRFrameGenerationSetting.h"

#include "Settings/LimenNvidiaFrameGenerationSetting.h"
#include "Subsystems/LimenModularSettingsSubsystem.h"


ULimenFSRFrameGenerationSetting::ULimenFSRFrameGenerationSetting()
{
	DevelopmentName = TEXT("setting_fsrframegen");
	Category = FText::FromString(TEXT("AMD"));
	DisplayName = FText::FromString(TEXT("FidelityFX FSR3 Frame Generation"));
	Description = FText::FromString(TEXT("Generates additional frames. Warning: FidelityFX FSR3 Frame Generation requires FSR3 upscaling to be enabled."));
	bCanEdit = true;
}

bool ULimenFSRFrameGenerationSetting::CanEdit() const
{
	if (!Super::CanEdit())
	{
		return false;
	}

	return bCanEdit;
}

void ULimenFSRFrameGenerationSetting::SubsystemInitialized(ULimenModularSettingsSubsystem* ModularSettingsSubsystem)
{
	Super::SubsystemInitialized(ModularSettingsSubsystem);

	ULimenNvidiaFrameGenerationSetting* AmdFsrUpscalingSetting = ModularSettingsSubsystem->GetItem<ULimenNvidiaFrameGenerationSetting>();
	AmdFsrUpscalingSetting->OnSettingUpdated.AddUniqueDynamic(this, &ThisClass::OnNvidiaFrameGenerationSettingChanged);
}

void ULimenFSRFrameGenerationSetting::SetDefaults()
{
	Super::SetDefaults();

	PossibleSelections.Reserve(2);
	PossibleSelections.Push(Enabled);
	PossibleSelections.Push(Disabled);
	DefaultSelection = Disabled;
}

void ULimenFSRFrameGenerationSetting::ApplyCurrentSetting()
{
	Super::ApplyCurrentSetting();

	IConsoleVariable* FrameInterpolation = IConsoleManager::Get().FindConsoleVariable(TEXT("r.FidelityFX.FI.Enabled"));
	if (GetCurrentValue().Compare(Enabled) == 0)
	{
		FrameInterpolation->Set(true, ECVF_SetByConsole);
	}
	else if (GetCurrentValue().Compare(Disabled) == 0)
	{
		FrameInterpolation->Set(false, ECVF_SetByConsole);
	}
	else
	{
		checkNoEntry();
	}
}

void ULimenFSRFrameGenerationSetting::OnNvidiaFrameGenerationSettingChanged(const ULimenSetting* Setting)
{
	const ULimenNvidiaFrameGenerationSetting* FrameGenerationSetting = Cast<ULimenNvidiaFrameGenerationSetting>(Setting);
	if (FrameGenerationSetting->GetCurrentValue().Compare(ULimenNvidiaFrameGenerationSetting::Enabled) == 0 && bCanEdit)
	{
		SetNewValue(Disabled);
		ApplyCurrentSetting();
		
		bCanEdit = false;
		OnSettingEditableStateChanged.Broadcast(this);
	}
	else if (FrameGenerationSetting->GetCurrentValue().Compare(ULimenNvidiaFrameGenerationSetting::Disabled) == 0 && !bCanEdit)
	{
		bCanEdit = true;
		OnSettingEditableStateChanged.Broadcast(this);
	}
}
