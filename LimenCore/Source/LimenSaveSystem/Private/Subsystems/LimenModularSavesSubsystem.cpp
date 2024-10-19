// Copyright Face Software. All Rights Reserved.


#include "Subsystems/LimenModularSavesSubsystem.h"

#include "Developer/LimenModularSavesDeveloperSettings.h"
#include "Kismet/GameplayStatics.h"
#include "SaveGames/LimenModularSaveData.h"
#include "SavesHandlers/LimenSavesHandler.h"
#include "Subsystems/LimenSaveSubsystem.h"


ULimenModularSavesSubsystem::ULimenModularSavesSubsystem()
{
	CurrentGameSaveData = nullptr;
	bHasLoadedGameData = false;
	bShouldLoadGameOnMapChange = false;
}

void ULimenModularSavesSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	Collection.InitializeDependency(ULimenSaveSubsystem::StaticClass());
	SaveSubsystem = ULimenSaveSubsystem::Get(GetWorld());
	check(SaveSubsystem != nullptr);

	FWorldDelegates::OnWorldInitializedActors.AddUObject(this, &ULimenModularSavesSubsystem::OnWorldActorsInitialized);

	FWorldDelegates::OnPostWorldInitialization.AddLambda([this] (UWorld*, UWorld::InitializationValues)
	{
		bHasLoadedGameData = false;
	});

	LoadToCurrentSaveData();
}

void ULimenModularSavesSubsystem::SaveCurrentGame(UObject* Caller)
{
	World = GEngine->GetWorldFromContextObject(Caller, EGetWorldErrorMode::Assert);
	SaveCurrentGame(World.Get());
}

void ULimenModularSavesSubsystem::LoadCurrentGame(UObject* Caller)
{
	World = GEngine->GetWorldFromContextObject(Caller, EGetWorldErrorMode::Assert);
	LoadCurrentGame(World.Get());
}

void ULimenModularSavesSubsystem::SaveCurrentGame(UWorld* InWorld)
{
	if (InWorld)
	{
		World = InWorld;
	}
	else
	{
		World = GetWorld();
	}
	check(IsValid(World.Get()));
	
	if (CurrentGameSaveData != nullptr)
	{
		CurrentGameSaveData->ConditionalBeginDestroy();
		CurrentGameSaveData = nullptr;
	}

	CurrentGameSaveData = NewObject<ULimenModularSaveData>();

	InitializeHandlersForSaving();
	SaveToCurrentSaveData();
}

void ULimenModularSavesSubsystem::LoadCurrentGame(UWorld* InWorld)
{
	if (InWorld)
	{
		World = InWorld;
	}
	else
	{
		World = GetWorld();
	}
	check(IsValid(World.Get()));

	LoadToCurrentSaveData();
	if (CurrentGameSaveData == nullptr)
	{
		SaveCurrentGame(World.Get());
		return;
	}
	InitializeHandlersForLoading();
}

void ULimenModularSavesSubsystem::ScheduleLoadOnMapChange()
{
	bShouldLoadGameOnMapChange = true;
}

bool ULimenModularSavesSubsystem::HasLoadedGameData() const
{
	return bHasLoadedGameData;
}

bool ULimenModularSavesSubsystem::DoesSavedDataExist() const
{
	return UGameplayStatics::DoesSaveGameExist(*SaveDataName.ToString(), 0);
}

void ULimenModularSavesSubsystem::DeleteSave()
{
	UGameplayStatics::DeleteGameInSlot(*SaveDataName.ToString(), 0);
}

bool ULimenModularSavesSubsystem::ShouldSaveData() const
{
	return true;
}

bool ULimenModularSavesSubsystem::ShouldLoadData() const
{
	return true;
}

void ULimenModularSavesSubsystem::DataSaved()
{
}

void ULimenModularSavesSubsystem::DataLoaded()
{
}

void ULimenModularSavesSubsystem::SaveToCurrentSaveData()
{
	SaveSubsystem->SaveData(CurrentGameSaveData.Get(), SaveDataName);
}

void ULimenModularSavesSubsystem::LoadToCurrentSaveData()
{
	CurrentGameSaveData = SaveSubsystem->LoadData<ULimenModularSaveData>(SaveDataName);
}

void ULimenModularSavesSubsystem::InitializeHandlersForSaving()
{
	CurrentGameSaveData->StoredSaveHandlers.Empty(0);
	const ULimenModularSavesDeveloperSettings* Settings = GetDefault<ULimenModularSavesDeveloperSettings>();
	for (const TSoftClassPtr<ULimenSavesHandler>& HandlerClass : Settings->SaveHandlers)
	{
		if (HandlerClass.IsNull())
		{
			continue;
		}
		
		ULimenSavesHandler* Handler = NewObject<ULimenSavesHandler>(this, HandlerClass.LoadSynchronous());
		Handler->SaveDataFrom(GetWorld());

		CurrentGameSaveData->StoredSaveHandlers.Push(FObjectSaveData(Handler));
		Handler->ConditionalBeginDestroy();
	}
}

void ULimenModularSavesSubsystem::InitializeHandlersForLoading()
{
	for (FObjectSaveData& HandlerData : CurrentGameSaveData->StoredSaveHandlers)
	{
		check(!HandlerData.GetObjectClass().IsNull());
		
		ULimenSavesHandler* Handler = NewObject<ULimenSavesHandler>(this, HandlerData.GetObjectClass().LoadSynchronous());
		HandlerData.LoadData(Handler); // Load the saved data to the handler
		
		Handler->LoadDataTo(GetWorld()); // Load the handler data to the world
		Handler->ConditionalBeginDestroy();
	}
}

void ULimenModularSavesSubsystem::OnWorldActorsInitialized(const FActorsInitializedParams& InitializationParams)
{
	if (bShouldLoadGameOnMapChange)
	{
		bHasLoadedGameData = true;
		if (InitializationParams.World->HasBegunPlay())
		{
			LoadCurrentGame(InitializationParams.World);
		}
		else
		{
			InitializationParams.World->OnWorldBeginPlay.AddLambda([this, InitializationParams]
			{
				LoadCurrentGame(InitializationParams.World);
			});
		}
		bShouldLoadGameOnMapChange = false;
	}
}
