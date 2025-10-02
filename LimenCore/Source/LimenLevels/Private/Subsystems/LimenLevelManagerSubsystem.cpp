// Copyright Face Software. All Rights Reserved.


#include "Subsystems/LimenLevelManagerSubsystem.h"

#include "GameMapsSettings.h"
#include "Developer/LimenLevelsDeveloperSettings.h"
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

#if !WITH_EDITOR
	UGameMapsSettings* Settings = GetMutableDefault<UGameMapsSettings>();
	if (const TSoftObjectPtr<UWorld> Level = ULimenLevelsDeveloperSettings::GetInitializationLevel(); !Level.IsNull())
	{
		Settings->SetGameDefaultMap(Level.GetLongPackageName());
		Settings->SaveConfig();
	}
#endif
}

void ULimenLevelManagerSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

int32 ULimenLevelManagerSubsystem::GetIndexOfGameLevel(const UWorld* InLevel)
{
	check(InLevel != nullptr);
	return ULimenLevelsDeveloperSettings::GetGameLevelIndex(InLevel);
}

bool ULimenLevelManagerSubsystem::OpenInitializationLevel()
{
	const TSoftObjectPtr<UWorld> Level = ULimenLevelsDeveloperSettings::GetInitializationLevel();
	if (Level.IsNull()) return false;

	const UWorld* LevelPtr = Level.LoadSynchronous();
	OpenOfflineLevel_Internal(LevelPtr->GetMapName());
	return true;
}

void ULimenLevelManagerSubsystem::OpenMainMenu(ELevelOpenContext Context)
{
	const TSoftObjectPtr<UWorld> Level = ULimenLevelsDeveloperSettings::GetMainMenuLevel();
	if (Level.IsNull()) return;

	const FString LevelPath = Level.ToSoftObjectPath().GetLongPackageName();
	switch (Context)
	{
	case ELevelOpenContext::Local:
		OpenOfflineLevel_Internal(LevelPath);
		break;

	case ELevelOpenContext::Connect:
		ConnectToServer_Internal(LevelPath);
		break;

	case ELevelOpenContext::Server:
		OpenServerLevel_Internal(LevelPath);
		break;
	}
}

void ULimenLevelManagerSubsystem::OpenGameEndLevel()
{
	OpenOfflineLevel_Internal(ULimenLevelsDeveloperSettings::GetGameEndLevel()->GetMapName());
}

void ULimenLevelManagerSubsystem::OpenGameLevel(ELevelOpenContext Context, const int32 Index, FString Options)
{
	if (!IsGameLevelIndexValid(Index)) return;

	const TSoftObjectPtr<UWorld> Level = ULimenLevelsDeveloperSettings::GetGameLevel(Index);
	if (Level.IsNull()) return;

	const FString LevelPath = Level.ToSoftObjectPath().GetLongPackageName();
	switch (Context)
	{
	case ELevelOpenContext::Local:
		OpenOfflineLevel_Internal(LevelPath, Options);
		break;
	case ELevelOpenContext::Connect:
		ConnectToServer_Internal(LevelPath, Options);
		break;
	case ELevelOpenContext::Server:
		OpenServerLevel_Internal(LevelPath, Options);
		break;
	}
}

void ULimenLevelManagerSubsystem::ResetCurrentLevel(ELevelOpenContext Context)
{
	switch (Context)
	{
	case ELevelOpenContext::Local:
		OpenOfflineLevel_Internal(GetWorld()->GetMapName());
		break;
	case ELevelOpenContext::Connect:
		ConnectToServer_Internal(GetWorld()->GetMapName());
		break;
	case ELevelOpenContext::Server:
		OpenServerLevel_Internal(GetWorld()->GetMapName());
		break;
	}
}

bool ULimenLevelManagerSubsystem::IsGameLevelIndexValid(const int32 Index) const
{
	const ULimenLevelsDeveloperSettings* Settings = GetDefault<ULimenLevelsDeveloperSettings>();
	check(Settings != nullptr);
	return Settings->IsGameLevelIndexValid(Index);
}

void ULimenLevelManagerSubsystem::OpenServerLevel_Internal(const FString& LevelName, const FString& Options)
{
	GetWorld()->ServerTravel(LevelName + "?listen"+ Options , false, false);
}

void ULimenLevelManagerSubsystem::ConnectToServer_Internal(const FString& IPAddress, const FString& Options)
{
	GetWorld()->GetFirstPlayerController()->ClientTravel(IPAddress + Options, TRAVEL_Absolute, false);
}

void ULimenLevelManagerSubsystem::OpenOfflineLevel_Internal(const FString& LevelName, const FString& Options)
{
	UGameplayStatics::OpenLevel(this, FName(LevelName), TRAVEL_Absolute, Options);
}
