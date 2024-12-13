// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "LimenLevelManagerSubsystem.generated.h"


DECLARE_DYNAMIC_DELEGATE_OneParam(FLevelLoadedDelegate, UWorld*, LoadedLevel);

/**
 * 
 */
UCLASS()
class LIMENLEVELS_API ULimenLevelManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	void OpenLocalLevelAsync(const TSoftObjectPtr<UWorld>& Level, FLevelLoadedDelegate& OnLevelLoaded);
	void OpenLocalLevel(const TSoftObjectPtr<UWorld>& Level);
	void OpenLocalLevel(const UWorld* Level);
	void OpenLocalLevel(const TSoftObjectPtr<UWorld>& Level, bool bAbsolute, const FString& InOptions);

	static int32 GetIndexOfGameLevel(const UWorld* InLevel);

	bool OpenInitializationLevel();
	UFUNCTION(BlueprintCallable, Category="Limen|Levels")
	void OpenMainMenu();
	UFUNCTION(BlueprintCallable, Category="Limen|Levels")
	void OpenGameLevel(const uint8 Index = 0);
	UFUNCTION(BlueprintCallable, Category="Limen|Levels")
	void ResetCurrentLevel();

private:
};
