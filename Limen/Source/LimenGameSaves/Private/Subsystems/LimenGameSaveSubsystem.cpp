// Copyright Face Software. All Rights Reserved.


#include "Subsystems/LimenGameSaveSubsystem.h"

#include "Developer/LimenGameSavesDeveloperSettings.h"
#include "Developer/LimenLevelsDeveloperSettings.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "SaveData/LimenGameSaveData.h"
#include "SavesHandlers/LimenSavesHandler.h"
#include "Subsystems/LimenLevelManagerSubsystem.h"
#include "Subsystems/LimenModalsSubsystem.h"
#include "Subsystems/LimenSaveSubsystem.h"
#include "Subsystems/SubsystemCollection.h"
#include "UMG/LimenGenericModalWidget.h"


const FString ULimenGameSaveSubsystem::GameSaveDataFolder = TEXT("GameSaves");
const FString ULimenGameSaveSubsystem::SaveDataName = TEXT("GameSlot01");

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

void ULimenGameSaveSubsystem::Deinitialize()
{
	Super::Deinitialize();

	FWorldDelegates::OnWorldInitializedActors.RemoveAll(this);
	FWorldDelegates::OnPostWorldInitialization.RemoveAll(this);
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
	CurrentGameSaveData->GameLevelIndex = ULimenLevelsDeveloperSettings::GetGameLevelIndex(World.Get());

	if (!InitializeHandlersForSaving() || CurrentGameSaveData->GameLevelIndex == INDEX_NONE)
	{
		DisplaySaveErrorModal();
		return;
	}
	
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
	
	if (!InitializeHandlersForLoading())
	{
		ULimenModalsSubsystem* ModalsSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<ULimenModalsSubsystem>();
		check(ModalsSubsystem != nullptr);

		const ULimenGameSavesDeveloperSettings* Settings = GetDefault<ULimenGameSavesDeveloperSettings>();

		ULimenGenericModalWidget* Modal = ModalsSubsystem->DisplayConfirmationModal(Settings->LoadGameFailedModalParams);
		check(Modal != nullptr)

		Modal->OnModalResponseReceived.AddDynamic(this, &ThisClass::FailedToLoadDataModalDismissed);
	}
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
	return UGameplayStatics::DoesSaveGameExist(GameSaveDataFolder / SaveDataName, 0);
}

void ULimenGameSaveSubsystem::DeleteGameSave()
{
	UGameplayStatics::DeleteGameInSlot(GameSaveDataFolder / SaveDataName, 0);
}

bool ULimenGameSaveSubsystem::ShouldSaveData() const
{
	return true;
}

bool ULimenGameSaveSubsystem::ShouldLoadData() const
{
	return true;
}

const ULimenGameSaveData* ULimenGameSaveSubsystem::GetCurrentGameSaveData() const
{
	return CurrentGameSaveData.Get();
}

void ULimenGameSaveSubsystem::DataSaved(const FString& SaveName, const int32 UserIndex, const bool bSuccess)
{
	CurrentSaveState = ESaveState::SavingComplete;
	OnGameSaveStateChanged.Broadcast(CurrentSaveState);
}

void ULimenGameSaveSubsystem::SaveToCurrentSaveData()
{
	CurrentSaveState = ESaveState::Saving;
	OnGameSaveStateChanged.Broadcast(CurrentSaveState);

	SaveSubsystem->SaveDataAsync(CurrentGameSaveData.Get(), GameSaveDataFolder / SaveDataName,
		[this] (const FString& SaveName, const int32 UserIndex, const bool bSuccess)
		{
			DataSaved(SaveName, UserIndex, bSuccess);
		});
}

void ULimenGameSaveSubsystem::LoadToCurrentSaveData()
{
	CurrentSaveState = ESaveState::Loading;
	OnGameSaveStateChanged.Broadcast(CurrentSaveState);

	CurrentGameSaveData = SaveSubsystem->LoadData<ULimenGameSaveData>(GameSaveDataFolder / SaveDataName);

	CurrentSaveState = ESaveState::LoadingComplete;
	OnGameSaveStateChanged.Broadcast(CurrentSaveState);
}

bool ULimenGameSaveSubsystem::InitializeHandlersForSaving()
{
	const ULimenGameSavesDeveloperSettings* Settings = GetDefault<ULimenGameSavesDeveloperSettings>();

	TArray<FObjectSaveData> HandlerData;
	HandlerData.Reserve(Settings->SaveHandlers.Num());

	for (auto& HandlerClass : Settings->SaveHandlers)
	{
		if (HandlerClass.IsNull()) { continue; }
		
		ULimenSavesHandler* Handler = NewObject<ULimenSavesHandler>(this, HandlerClass.LoadSynchronous());
		if (!Handler->SaveDataFrom(GetWorld()))
		{
			Handler->ConditionalBeginDestroy();
			return false;
		}

		HandlerData.Push(FObjectSaveData(Handler));
		Handler->ConditionalBeginDestroy();
	}

	CurrentGameSaveData->StoredSaveHandlers = MoveTemp(HandlerData);
	return true;
}

bool ULimenGameSaveSubsystem::InitializeHandlersForLoading()
{
	for (FObjectSaveData& HandlerData : CurrentGameSaveData->StoredSaveHandlers)
	{
		check(!HandlerData.GetObjectClass().IsNull());

		const UClass* HandlerClass = HandlerData.GetObjectClass().TryLoadClass<ULimenSavesHandler>();
		if (!HandlerClass) return false;
		
		ULimenSavesHandler* Handler = NewObject<ULimenSavesHandler>(this, HandlerClass);
		HandlerData.LoadData(Handler); // Load the saved data to the handler
		
		if (!Handler->LoadDataTo(GetWorld())) // Load the handler data to the world
		{
			Handler->ConditionalBeginDestroy();
			return false;
		}

		Handler->ConditionalBeginDestroy();
	}

	return true;
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

void ULimenGameSaveSubsystem::FailedToLoadDataModalDismissed(ULimenGenericModalWidget* ModalWidget, bool bAccepted)
{
	ModalWidget->OnModalResponseReceived.RemoveDynamic(this, &ThisClass::FailedToLoadDataModalDismissed);

	ULimenLevelManagerSubsystem* LevelManager = GetWorld()->GetGameInstance()->GetSubsystem<ULimenLevelManagerSubsystem>();
	check(LevelManager != nullptr);

	LevelManager->OpenMainMenu();
}

void ULimenGameSaveSubsystem::FailedToSaveDataModalDismissed(ULimenGenericModalWidget* ModalWidget, bool bAccepted)
{
	ModalWidget->OnModalResponseReceived.RemoveDynamic(this, &ThisClass::FailedToSaveDataModalDismissed);
}

void ULimenGameSaveSubsystem::DisplaySaveErrorModal()
{
	ULimenModalsSubsystem* ModalsSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<ULimenModalsSubsystem>();
	check(ModalsSubsystem != nullptr);

	const ULimenGameSavesDeveloperSettings* Settings = GetDefault<ULimenGameSavesDeveloperSettings>();
	ULimenGenericModalWidget* Modal = ModalsSubsystem->DisplayConfirmationModal(Settings->SaveGameFailedModalParams);

	if (!Modal) { return; }
	Modal->OnModalResponseReceived.AddDynamic(this, &ThisClass::FailedToSaveDataModalDismissed);
}
