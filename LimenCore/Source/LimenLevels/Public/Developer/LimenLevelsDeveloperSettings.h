// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "LimenLevelsDeveloperSettings.generated.h"

/**
 * 
 */
UCLASS(Config="Limen")
class LIMENLEVELS_API ULimenLevelsDeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	static TSoftObjectPtr<UWorld> GetInitializationLevel();
	static TSoftObjectPtr<UWorld> GetMainMenuLevel();
	static TSoftObjectPtr<UWorld> GetGameEndLevel();
	static TSoftObjectPtr<UWorld> GetGameLevel(const int32 Index);
	static int32 GetGameLevelIndex(const FSoftObjectPath& Level);
	static int32 GetGameLevelIndex(UWorld* Level);
	static bool IsGameLevelIndexValid(const int32 Index);
	static bool ShouldUseSubsystem();

public:	
	ULimenLevelsDeveloperSettings();
	virtual FName GetContainerName() const override;
#if WITH_EDITOR
	virtual FText GetSectionText() const override;
#endif


protected:
	UPROPERTY(EditAnywhere, Config, Category="Limen")
	bool bUseSubsystem;
	
	UPROPERTY(EditAnywhere, Config,Category="Limen")
	TSoftObjectPtr<UWorld> InitializationLevel;
	
	UPROPERTY(EditAnywhere, Config,Category="Limen")
	TSoftObjectPtr<UWorld> MainMenuLevel;
	
	UPROPERTY(EditAnywhere, Config,Category="Limen")
	TSoftObjectPtr<UWorld> GameEndLevel;
	
	UPROPERTY(EditAnywhere, Config,Category="Limen")
	TArray<TSoftObjectPtr<UWorld>> GameLevels;
};
