// Copyright Face Software. All Rights Reserved.


#include "Settings/LimenDynamicDepthOfFieldSetting.h"

#include "Subsystems/LimenGraphicalSettingsSubsystem.h"


ULimenDynamicDepthOfFieldSetting::ULimenDynamicDepthOfFieldSetting()
{
	DevelopmentName = TEXT("setting_dyanmicdepthoffield");
	Category = FText::FromString(TEXT("Post Process FX"));
	DisplayName = FText::FromString(TEXT("Depth of Field"));
	Description = FText::FromString(TEXT("Adds a level of realism by only focusing on what the camera is looking at."));
}

void ULimenDynamicDepthOfFieldSetting::ApplyCurrentSetting()
{
	Super::ApplyCurrentSetting();

	auto* GraphicalSettings = GetWorld()->GetGameInstance()->GetSubsystem<ULimenGraphicalSettingsSubsystem>();
	if (auto* GlobalPostProcess = GraphicalSettings->GetGlobalPostProcess(); GlobalPostProcess != nullptr)
	{
		GlobalPostProcessFound(GlobalPostProcess);
	}
	else
	{
		GraphicalSettings->OnGlobalPostProcessFound.AddUObject(this, &ThisClass::GlobalPostProcessFound);
	}
}

void ULimenDynamicDepthOfFieldSetting::SetDefaults()
{
	Super::SetDefaults();

	PossibleSelections.Reserve(2);
	PossibleSelections.Push(Enabled);
	PossibleSelections.Push(Disabled);

	DefaultSelection = Enabled;
}

void ULimenDynamicDepthOfFieldSetting::GlobalPostProcessFound(APostProcessVolume* PostProcess)
{
	const bool bShouldActivate = GetCurrentValue() == Enabled;
	PostProcess->Settings.bOverride_DepthOfFieldScale = bShouldActivate;
	PostProcess->Settings.bOverride_DepthOfFieldFocalDistance = bShouldActivate;
}
