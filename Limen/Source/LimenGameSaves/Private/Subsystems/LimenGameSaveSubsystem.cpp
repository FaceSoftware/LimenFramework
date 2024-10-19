// Copyright Face Software. All Rights Reserved.


#include "Subsystems/LimenGameSaveSubsystem.h"

#include "Developer/LimenGameSavesDeveloperSettings.h"
#include "Kismet/GameplayStatics.h"
#include "SaveData/LimenGameSaveData.h"
#include "SavesHandlers/LimenSavesHandler.h"
#include "Subsystems/LimenSaveSubsystem.h"


ULimenGameSaveSubsystem::ULimenGameSaveSubsystem()
{
	CurrentGameSaveData = nullptr;
	bHasLoadedGameData = false;
	bShouldLoadGameOnMapChange = false;
}

void ULimenGameSaveSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	Collection.InitializeDependency(ULimenSaveSubsystem::StaticClass());
	SaveSubsystem = ULimenSaveSubsystem::Get(GetWorld());
	check(SaveSubsystem != nullptr);

	FWorldDelegates::OnWorldInitializedActors.AddUObject(this, &ULimenGameSaveSubsystem::OnWorldActorsInitialized);

	FWorldDelegates::OnPostWorldInitialization.AddLambda([this] (UWorld*, UWorld::InitializationValues)
	{
		bHasLoadedGameData = false;
	});

	LoadToCurrentSaveData();
}

void ULimenGameSaveSubsystem::SaveCurrentGame(UObject* Caller)
{
	World = GEngine->GetWorldFromContextObject(Caller, EGetWorldErrorMode::Assert);
	SaveCurrentGame(World.Get());
}

void ULimenGameSaveSubsystem::LoadCurrentGame(UObject* Caller)
{
	World = GEngine->GetWorldFromContextObject(Caller, EGetWorldErrorMode::Assert);
	LoadCurrentGame(World.Get());
}

void ULimenGameSaveSubsystem::SaveCurrentGame(UWorld* InWorld)
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

	CurrentGameSaveData = NewObject<ULimenGameSaveData>();

	InitializeHandlersForSaving();
	SaveToCurrentSaveData();
}

void ULimenGameSaveSubsystem::LoadCurrentGame(UWorld* InWorld)
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

void ULimenGameSaveSubsystem::ScheduleGameLoadOnMapChange()
{
	bShouldLoadGameOnMapChange = true;
}

bool ULimenGameSaveSubsystem::HasLoadedGameData() const
{
	return bHasLoadedGameData;
}

bool ULimenGameSaveSubsystem::DoesSavedDataExist() const
{
	return UGameplayStatics::DoesSaveGameExist(*SaveDataName.ToString(), 0);
}

void ULimenGameSaveSubsystem::DeleteGameSave()
{
	UGameplayStatics::DeleteGameInSlot(*SaveDataName.ToString(), 0);
}

bool ULimenGameSaveSubsystem::ShouldSaveData() const
{
	return true;
}

bool ULimenGameSaveSubsystem::ShouldLoadData() const
{
	return true;
}

void ULimenGameSaveSubsystem::DataSaved()
{
}

void ULimenGameSaveSubsystem::DataLoaded()
{
}

void ULimenGameSaveSubsystem::SaveToCurrentSaveData()
{
	SaveSubsystem->SaveData(CurrentGameSaveData.Get(), SaveDataName);
}

void ULimenGameSaveSubsystem::LoadToCurrentSaveData()
{
	CurrentGameSaveData = SaveSubsystem->LoadData<ULimenGameSaveData>(SaveDataName);
}

void ULimenGameSaveSubsystem::InitializeHandlersForSaving()
{
	CurrentGameSaveData->StoredSaveHandlers.Empty(0);
	const ULimenGameSavesDeveloperSettings* Settings = GetDefault<ULimenGameSavesDeveloperSettings>();
	for (auto& HandlerClass : Settings->SaveHandlers)
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

void ULimenGameSaveSubsystem::InitializeHandlersForLoading()
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

void ULimenGameSaveSubsystem::OnWorldActorsInitialized(const FActorsInitializedParams& InitializationParams)
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
