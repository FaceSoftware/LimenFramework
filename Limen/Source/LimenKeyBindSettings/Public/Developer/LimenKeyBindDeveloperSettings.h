// Copyright © 2024 FaceSoftware. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Developer/LimenModularSettingsSubsystemDeveloperSettings.h"
#include "MappableKeySettings/LimenPlayerMappableKeySettings.h"
#include "LimenKeyBindDeveloperSettings.generated.h"


enum class EInputActionHandlingType : uint8;
class UInputTrigger;
class UInputAction;
class UInputMappingContext;

/**
 * 
 */
UCLASS(Config="Limen")
class LIMENKEYBINDSETTINGS_API ULimenKeyBindDeveloperSettings : public ULimenModularSettingsSubsystemDeveloperSettings
{
	GENERATED_BODY()
	
	using FMappingContextSoftPtr = TSoftObjectPtr<UInputMappingContext>;
	using FPawnClassSoftPtr = TSoftClassPtr<APawn>;
	using FPlayerControllerClassSoftPtr = TSoftClassPtr<APlayerController>;

public:
	UPROPERTY(EditDefaultsOnly, Config)
	TMap<TSoftClassPtr<APlayerController>, TSoftObjectPtr<UInputMappingContext>> PlayerMappingContexts;
	UPROPERTY(EditDefaultsOnly, Config)
	TMap<TSoftClassPtr<APawn>, TSoftObjectPtr<UInputMappingContext>> PawnMappingContexts;
	
	
	ULimenKeyBindDeveloperSettings()
	{
		SectionName = TEXT("Limen - Key Bind Settings");
	}

	TArray<FMappingContextSoftPtr> GetAllInputMappingContexts() const;
};
