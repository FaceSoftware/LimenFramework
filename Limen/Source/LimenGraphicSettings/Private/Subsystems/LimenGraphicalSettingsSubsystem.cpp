// Copyright Face Software. All Rights Reserved.


#include "Subsystems/LimenGraphicalSettingsSubsystem.h"

#include "Developer/LimenGraphicalSettingsDeveloperSettings.h"
#include "LogMacros/LimenLogMacros.h"
#include "Settings/LimenSetting.h"


ULimenGraphicalSettingsSubsystem::ULimenGraphicalSettingsSubsystem()
{
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

void ULimenGraphicalSettingsSubsystem::PlayerControllerChanged(APlayerController* NewPlayerController)
{
	Super::PlayerControllerChanged(NewPlayerController);
	
	if (!NewPlayerController) { return; }

	if (APlayerCameraManager* CameraManager = NewPlayerController->PlayerCameraManager.Get();
		ensureAlways(CameraManager != nullptr))
	{
		if (SubsystemSettings.IsValid() && ensureAlways(!SubsystemSettings->CameraModifierClass.IsNull()))
		{
			auto* Modifier = CameraManager->AddNewCameraModifier(SubsystemSettings->CameraModifierClass.LoadSynchronous());
			CameraModifier = CastChecked<ULimenGraphicalSettingsCameraModifier>(Modifier);
			CameraModifier->BindSubsystem(this);
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
