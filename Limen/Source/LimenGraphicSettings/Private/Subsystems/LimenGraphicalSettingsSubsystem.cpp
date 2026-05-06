// Copyright Face Software. All Rights Reserved.


#include "Subsystems/LimenGraphicalSettingsSubsystem.h"

#include "EngineUtils.h"
#include "TimerManager.h"
#include "Developer/LimenGraphicalSettingsDeveloperSettings.h"
#include "Engine/PostProcessVolume.h"
#include "Kismet/GameplayStatics.h"
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

	const auto* DeveloperSettings = GetDefault<ULimenGraphicalSettingsDeveloperSettings>();
	return DeveloperSettings->bUseSubsystem;
}

APostProcessVolume* ULimenGraphicalSettingsSubsystem::GetGlobalPostProcess() const
{
	return GlobalPostProcess.Get();
}

void ULimenGraphicalSettingsSubsystem::WorldInitializedActors(const FActorsInitializedParams& InitParams)
{
	Super::WorldInitializedActors(InitParams);
	
	GetWorld()->GetTimerManager().ClearTimer(FindPostProcessHandle);
	FindGlobalPostProcessVolume(InitParams.World, SubsystemSettings->GlobalPostProcessTag);
	
	APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(this, 0);
	if (ensureAlways(CameraManager != nullptr))
	{
		const auto* DeveloperSettings = GetDefault<ULimenGraphicalSettingsDeveloperSettings>();
		if (ensureAlways(!DeveloperSettings->CameraModifierClass.IsNull()))
		{
			auto* Modifier = CameraManager->AddNewCameraModifier(DeveloperSettings->CameraModifierClass.LoadSynchronous());
			CameraModifier = CastChecked<ULimenGraphicalSettingsCameraModifier>(Modifier);
		}
	}
	
}

void ULimenGraphicalSettingsSubsystem::LoadDefaultSettingsList()
{
	Super::LoadDefaultSettingsList();

	SubsystemSettings = GetDefault<ULimenGraphicalSettingsDeveloperSettings>();
	for (const TSoftClassPtr<ULimenSetting>& SettingClass : SubsystemSettings->SettingsList)
	{
		if (!ensureAlways(!SettingClass.IsNull()))
		{
			// Don't crash the editor if the class is not set
			continue;
		}

		LIMEN_LOG(LogLimen, Log, this, TEXT("Loading setting: %s"), *SettingClass.ToSoftObjectPath().ToString());
		
		ULimenSetting* NewSetting = NewObject<ULimenSetting>(this, SettingClass.LoadSynchronous());
		check(NewSetting != nullptr);
		AddItem(NewSetting);
	}
	
	for (ULimenSetting* Setting : GetItems<ULimenSetting>())
	{
		Setting->InitializeSetting(this);
	}
}

void ULimenGraphicalSettingsSubsystem::FindGlobalPostProcessVolume(const UWorld* World, const FName& Tag)
{
	for (APostProcessVolume* PostProcess : TActorRange<APostProcessVolume>(World))
	{
		if (PostProcess->ActorHasTag(Tag))
		{
			GlobalPostProcess = PostProcess;
			OnGlobalPostProcessFound.Broadcast(GlobalPostProcess.Get());
			return;
		}
	}

	FindPostProcessHandle = GetWorld()->GetTimerManager().SetTimerForNextTick([this, World, Tag]
	{
		FindGlobalPostProcessVolume(World, Tag);
	});
}
