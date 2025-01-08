// Copyright Face Software. All Rights Reserved.


#include "Subsystems/LimenLevelManagerSubsystem.h"

#include "Developer/LimenLevelsDeveloperSettings.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"


bool ULimenLevelManagerSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	if (!Super::ShouldCreateSubsystem(Outer))
	{
		return false;
	}

	return ULimenLevelsDeveloperSettings::ShouldUseSubsystem();
}

void ULimenLevelManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void ULimenLevelManagerSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void ULimenLevelManagerSubsystem::OpenLocalLevelAsync(const TSoftObjectPtr<UWorld>& Level, FLevelLoadedDelegate& OnLevelLoaded)
{
	check(!Level.IsNull());
	FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
	
	Streamable.RequestAsyncLoad(Level.ToSoftObjectPath(), [this, Level, OnLevelLoaded]
	{
		UWorld* LoadedLevel = Level.Get();
		check(LoadedLevel != nullptr);
		
		UGameplayStatics::OpenLevel(GetWorld(), LoadedLevel->GetFName());
		OnLevelLoaded.ExecuteIfBound(LoadedLevel);
		
	}, FStreamableManager::DefaultAsyncLoadPriority);
}

void ULimenLevelManagerSubsystem::OpenLocalLevel(const TSoftObjectPtr<UWorld>& Level)
{
	check(!Level.IsNull());
	OpenLocalLevel(Level, true, TEXT(""));
}

void ULimenLevelManagerSubsystem::OpenLocalLevel(const UWorld* Level)
{
	UGameplayStatics::OpenLevel(this, *Level->GetMapName());
}

void ULimenLevelManagerSubsystem::OpenLocalLevel(const TSoftObjectPtr<UWorld>& Level, bool bAbsolute, const FString& InOptions)
{
	check(!Level.IsNull());
	UGameplayStatics::OpenLevelBySoftObjectPtr(this, Level, bAbsolute, InOptions);
}

int32 ULimenLevelManagerSubsystem::GetIndexOfGameLevel(const UWorld* InLevel)
{
	check(InLevel != nullptr);
	return static_cast<int32>(ULimenLevelsDeveloperSettings::GetGameLevelIndex(InLevel));
}

bool ULimenLevelManagerSubsystem::OpenInitializationLevel()
{
	if (const TSoftObjectPtr<UWorld> Level = ULimenLevelsDeveloperSettings::GetInitializationLevel(); !Level.IsNull())
	{
		OpenLocalLevel(Level);
		return true;
	}

	return false;
}

void ULimenLevelManagerSubsystem::OpenMainMenu()
{
	OpenLocalLevel(ULimenLevelsDeveloperSettings::GetMainMenuLevel());
}

void ULimenLevelManagerSubsystem::OpenGameEndLevel()
{
	OpenLocalLevel(ULimenLevelsDeveloperSettings::GetGameEndLevel());
}

void ULimenLevelManagerSubsystem::OpenGameLevel(const uint8 Index)
{
	OpenLocalLevel(ULimenLevelsDeveloperSettings::GetGameLevel(Index));
}

void ULimenLevelManagerSubsystem::ResetCurrentLevel()
{
	OpenLocalLevel(GetWorld());
}
