// Copyright Face Software. All Rights Reserved.


#include "Settings/LimenBlendableSetting.h"

#include "TimerManager.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Subsystems/LimenGraphicalSettingsSubsystem.h"


ULimenBlendableSetting::ULimenBlendableSetting()
{
	DevelopmentName = TEXT("setting_blendable");
	Category = FText::FromString(TEXT("Post Process FX"));
	DisplayName = FText::FromString(TEXT("Blendable Name"));
	Description = FText::FromString(TEXT("Blendable Description."));
	BlendableIndex = 0;
}

void ULimenBlendableSetting::SetDefaults()
{
	Super::SetDefaults();

	PossibleSelections.Reserve(2);
	PossibleSelections.Push(Enabled);
	PossibleSelections.Push(Disabled);
	DefaultSelection = Enabled;
	
	if (auto* Subsystem = GetOwnerSubsystem<ULimenGraphicalSettingsSubsystem>())
	{
		Subsystem->OnPostProcessSettingEvaluate.AddUObject(this, &ThisClass::PostProcessSettingEvaluate);
	}
}

void ULimenBlendableSetting::PostProcessSettingEvaluate(FPostProcessSettings& Settings)
{
	if (TArray<FWeightedBlendable>& List = Settings.WeightedBlendables.Array;
		List.IsValidIndex(BlendableIndex))
	{
		List[BlendableIndex].Weight = GetAppliedValue() == Enabled ? 1.f : 0.f;
	}
}
