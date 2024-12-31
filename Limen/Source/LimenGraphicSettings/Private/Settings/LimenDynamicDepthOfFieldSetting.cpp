// Copyright Face Software. All Rights Reserved.


#include "Settings/LimenDynamicDepthOfFieldSetting.h"

#include "EngineUtils.h"
#include "Components/LimenDynamicDepthOfFieldComponent.h"
#include "Subsystems/LimenGraphicalSettingsSubsystem.h"


ULimenDynamicDepthOfFieldSetting::ULimenDynamicDepthOfFieldSetting()
{
	bUseRecurrentAction = false;
	DevelopmentName = TEXT("setting_ddof");
	Category = FText::FromString(TEXT("Post Process FX"));
	DisplayName = FText::FromString(TEXT("Depth of Field"));
	Description = FText::FromString(TEXT("Adds a level of realism by only focusing on what the camera is looking at."));
}

void ULimenDynamicDepthOfFieldSetting::ApplyCurrentSetting(const bool bUserRequest)
{
	Super::ApplyCurrentSetting();

	for (TActorIterator<AActor> It(GetWorld()); It; ++It)
	{
		TArray<ULimenDynamicDepthOfFieldComponent*> DoFComponents;
		It->GetComponents<ULimenDynamicDepthOfFieldComponent>(DoFComponents);
		
		for (auto* Component : DoFComponents)
		{
			check(GetCurrentValue() == Enabled || GetCurrentValue() == Disabled)
			Component->SetActive(GetCurrentValue() == Enabled);
		}
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
