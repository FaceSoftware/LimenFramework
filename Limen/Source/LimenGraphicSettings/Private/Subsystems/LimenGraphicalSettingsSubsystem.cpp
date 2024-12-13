// Copyright Face Software. All Rights Reserved.


#include "Subsystems/LimenGraphicalSettingsSubsystem.h"

#include "EngineUtils.h"
#include "BlueprintAsyncActions/LimenRecurrentAction.h"
#include "Developer/LimenGraphicalSettingsDeveloperSettings.h"
#include "Engine/PostProcessVolume.h"
#include "LogMacros/LimenLogMacros.h"
#include "Settings/LimenSetting.h"


ULimenGraphicalSettingsSubsystem::ULimenGraphicalSettingsSubsystem()
{
	GlobalPostProcess = nullptr;
	SaveDataName = TEXT("GraphicSettings");
}

bool ULimenGraphicalSettingsSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	if (!Super::ShouldCreateSubsystem(Outer))
	{
		return false;
	}

	// We cannot set the SubsystemSettings variable here because this function is constant
	// Just use a temp, it's ugly but no one will notice...
	const auto* DeveloperSettings = GetDefault<ULimenGraphicalSettingsDeveloperSettings>();
	return DeveloperSettings->bUseSubsystem;
}

void ULimenGraphicalSettingsSubsystem::LoadDefaultSettingsList()
{
	Super::LoadDefaultSettingsList();

	SubsystemSettings = GetDefault<ULimenGraphicalSettingsDeveloperSettings>();
	for (const TSoftClassPtr<ULimenSetting>& SettingClass : SubsystemSettings->SettingsList)
	{
		if (SettingClass.IsNull())
		{
			// Don't crash the editor if the class is not set
			continue;
		}

		LIMEN_LOG(LogLimen, Log, this, "Loading setting: %s", *SettingClass.ToSoftObjectPath().ToString());
		
		ULimenSetting* NewSetting = NewObject<ULimenSetting>(this, SettingClass.LoadSynchronous());
		check(NewSetting != nullptr);
		AddItem(NewSetting);
	}
	
	for (ULimenSetting* Setting : GetItems<ULimenSetting>())
	{
		Setting->InitializeSetting();
	}
}

APostProcessVolume* ULimenGraphicalSettingsSubsystem::GetGlobalPostProcess()
{
	if (GlobalPostProcess != nullptr)
	{
		return GlobalPostProcess.Get();
	}

	if (auto* PostProcess = FindGlobalPostProcessVolume(GetWorld(), SubsystemSettings->GlobalPostProcessTag);
		PostProcess != nullptr)
	{
		GlobalPostProcess = PostProcess;
	}

	return GlobalPostProcess.Get();
}

void ULimenGraphicalSettingsSubsystem::PostWorldInitialization(UWorld* World,
	const UWorld::InitializationValues InitValues)
{
	Super::PostWorldInitialization(World, InitValues);

	FRecurrentActionStopCondition StopCondition;
	StopCondition.BindDynamic(this, &ThisClass::StopRecurrentAction);
	FRecurrentActionDelegate Action;
	Action.BindDynamic(this, &ThisClass::FindGlobalPostProcessVolumeWrapper);
	
	FindGlobalPostProcessAction = ULimenRecurrentAction::StartRecurrentAction(World, Action, 0, .5f, StopCondition);
	FindGlobalPostProcessAction->OnSuccess.AddUniqueDynamic(this, &ThisClass::GlobalPostProcessFound);
	FindGlobalPostProcessAction->Activate();
}

bool ULimenGraphicalSettingsSubsystem::StopRecurrentAction()
{
	return IsValid(GlobalPostProcess.Get());
}

APostProcessVolume* ULimenGraphicalSettingsSubsystem::FindGlobalPostProcessVolume(const UWorld* World, const FName Tag)
{
	for (TActorIterator<APostProcessVolume> It(World); It; ++It)
	{
		if (It->Tags.Contains(Tag))
		{
			return *It;
		}
	}

	return nullptr;
}

void ULimenGraphicalSettingsSubsystem::FindGlobalPostProcessVolumeWrapper()
{
	GlobalPostProcess = FindGlobalPostProcessVolume(GetWorld(), SubsystemSettings->GlobalPostProcessTag);
}

void ULimenGraphicalSettingsSubsystem::GlobalPostProcessFound()
{
	if (FindGlobalPostProcessAction)
	{
		FindGlobalPostProcessAction->ConditionalBeginDestroy();
		FindGlobalPostProcessAction = nullptr;
		OnGlobalPostProcessFound.Broadcast(GlobalPostProcess.Get());
	}
}
