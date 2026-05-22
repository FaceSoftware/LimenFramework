// Copyright © 2024 FaceSoftware. All rights reserved.


#include "Components/LimenModularSettingListener.h"

#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "Subsystems/LimenModularSettingsSubsystem.h"


ULimenModularSettingListener::ULimenModularSettingListener()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void ULimenModularSettingListener::Activate(const bool bReset)
{
	Super::Activate(bReset);
	
	if (auto* PC = GetOwner<APlayerController>(); PC && (bReset || ShouldActivate()==true))
	{
		if (const auto* LocalPlayer = PC->GetLocalPlayer(); LocalPlayer && !ModularSettingsSubsystem.IsValid())
		{
			ModularSettingsSubsystem = LocalPlayer->GetSubsystem<ULimenModularSettingsSubsystem>();
			ModularSettingsSubsystem->OnSettingUpdated.AddUniqueDynamic(this, &ThisClass::SettingUpdated);
		}
	}
}

void ULimenModularSettingListener::Deactivate()
{
	if (ShouldActivate()==false)
	{
		check(ModularSettingsSubsystem.IsValid())
		ModularSettingsSubsystem->OnSettingUpdated.RemoveDynamic(this, &ThisClass::SettingUpdated);
	}
	
	Super::Deactivate();
}

void ULimenModularSettingListener::SettingUpdated(const ULimenSetting* Setting)
{
	OnSettingUpdated.Broadcast(Setting);
}
