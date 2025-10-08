// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "LimenLevelManagerSubsystem.generated.h"


enum ETravelType : int;
DECLARE_DYNAMIC_DELEGATE_OneParam(FLevelLoadedDelegate, UWorld*, LoadedLevel);

UENUM(BlueprintType)
enum class ELevelOpenContext : uint8
{
	// Local map, offline play
	Local,
	// Connect to a server
	Connect,
	// Server level change
	Server,
};

/**
 * 
 */
UCLASS()
class LIMENLEVELS_API ULimenLevelManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	static int32 GetIndexOfGameLevel(const UWorld* InLevel);

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable, Category="Limen|Levels")
	bool OpenInitializationLevel();
	UFUNCTION(BlueprintCallable, Category="Limen|Levels")
	void OpenMainMenu();
	UFUNCTION(BlueprintCallable, Category="Limen|Levels")
	void OpenGameEndLevel();
	UFUNCTION(BlueprintCallable, Category="Limen|Levels")
	void OpenGameLevel(ELevelOpenContext Context = ELevelOpenContext::Local, const int32 Index = 0, FString Options = TEXT(""));
	UFUNCTION(BlueprintCallable, Category="Limen|Levels")
	void ResetCurrentLevel(ELevelOpenContext Context = ELevelOpenContext::Local);

	bool IsGameLevelIndexValid(const int32 Index) const;

private:
	void OpenServerLevel_Internal(const FString& LevelName, const FString& Options = TEXT(""));
	void ConnectToServer_Internal(const FString& IPAddress, const FString& Options = TEXT(""));
	void OpenOfflineLevel_Internal(const FString& LevelName, const FString& Options = TEXT(""));
};
