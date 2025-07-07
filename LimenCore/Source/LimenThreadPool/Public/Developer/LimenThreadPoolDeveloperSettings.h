// Copyright © 2024 FaceSoftware. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "LimenThreadPoolDeveloperSettings.generated.h"

UENUM()
enum class EBP_ThreadPriority : uint8
{
	Normal					= 0x000,
	AboveNormal				= 0x001,
	BelowNormal				= 0x002,
	Highest					= 0x003,
	Lowest					= 0x004,
	SlightlyBelowNormal		= 0x005,
	TimeCritical			= 0x006,
};

/**
 * @brief WINDOWS ONLY!
 */
UCLASS(Config="Limen")
class LIMENTHREADPOOL_API ULimenThreadPoolDeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	ULimenThreadPoolDeveloperSettings();
	
	UPROPERTY(EditAnywhere, Config)
	bool bUseSubsystem;
	UPROPERTY(EditAnywhere, Config)
	bool bAutoDetermineThreadCount;
	UPROPERTY(EditAnywhere, Config, meta=(EditCondition="!bAutoDetermineThreadCount", ClampMin="0"))
	int32 NumThreadsToUse;
	UPROPERTY(EditAnywhere, Config, meta=(ClampMin="0"))
	int32 MemoryPerThreadInBytes;
	UPROPERTY(EditAnywhere, Config, meta=(ClampMin="0"))
	EBP_ThreadPriority ThreadsPriority;

	int32 GetThreadCount() const;

	virtual FName GetContainerName() const override;
	virtual FName GetCategoryName() const override;
	virtual FName GetSectionName() const override;
#if WITH_EDITOR
	virtual FText GetSectionText() const override;
	virtual FText GetSectionDescription() const override;
#endif
};
