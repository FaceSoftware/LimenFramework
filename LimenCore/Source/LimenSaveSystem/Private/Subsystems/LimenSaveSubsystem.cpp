// Copyright Face Software. All Rights Reserved.


#include "Subsystems/LimenSaveSubsystem.h"

#include "Kismet/GameplayStatics.h"
#include "SaveGames/LimenSaveData.h"


void ULimenSaveSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

ULimenSaveSubsystem* ULimenSaveSubsystem::Get(const UWorld* World)
{
	const auto* GameInstanceSubsystem = World->GetGameInstance();
	check(GameInstanceSubsystem);

	auto* SaveSubsystem = World->GetGameInstance()->GetSubsystem<ULimenSaveSubsystem>();
	check(SaveSubsystem)

	return SaveSubsystem;
}

bool ULimenSaveSubsystem::SaveData(ULimenSaveData* SaveData, const FString& DataName)
{
	if (SaveData == nullptr || DataName.IsEmpty())
	{
		return false;
	}

	return UGameplayStatics::SaveGameToSlot(SaveData, DataName, 0);
}

bool ULimenSaveSubsystem::SaveDataAsync(ULimenSaveData* SaveData, const FString& DataName, const TFunction<void(const FString&, const int32, const bool)>& SaveFinishCallback)
{
	if (SaveData == nullptr || DataName.IsEmpty())
	{
		return false;
	}

	// Do not pass the "SaveFinishCallback" by reference, pass it by copy,
	// or the variable will go out of scope causing the reference to be lost...
	FAsyncSaveGameToSlotDelegate SaveDelegate;
	SaveDelegate.BindLambda([this, SaveFinishCallback] (const FString& StringDataName, const int32 UserIndex, const bool bSuccess)
	{
		SaveFinishCallback(StringDataName, UserIndex, bSuccess);
		OnSaveStateChanged.Broadcast(ESaveState::SavingComplete);
		bIsAsyncOperationInProgress = false;
	});

	bIsAsyncOperationInProgress = true;
	OnSaveStateChanged.Broadcast(ESaveState::Saving);
	UGameplayStatics::AsyncSaveGameToSlot(SaveData, DataName, 0, SaveDelegate);
	return true;
}

ULimenSaveData* ULimenSaveSubsystem::LoadData(const FString& DataName)
{
	if (!UGameplayStatics::DoesSaveGameExist(DataName, 0))
	{
		return nullptr;
	}

	USaveGame* LoadedData = UGameplayStatics::LoadGameFromSlot(DataName, 0);
	return LoadedData == nullptr ? nullptr : Cast<ULimenSaveData>(LoadedData);
}

bool ULimenSaveSubsystem::LoadDataAsync(const FName& DataName, const TFunction<void(const FString&, const int32, ULimenSaveData*)>& LoadFinishCallback)
{
	if (DataName.GetStringLength() == 0)
	{
		return false;
	}

	// Do not pass the "LoadFinishCallback" by reference, pass it by copy, or the variable will go out of scope and reference nothing...
	FAsyncLoadGameFromSlotDelegate LoadDelegate;
	LoadDelegate.BindLambda([this, LoadFinishCallback] (const FString& StringDataName, const int32 UserIndex, USaveGame* LoadedData)
	{
		auto* Data = Cast<ULimenSaveData>(LoadedData);
		LoadFinishCallback(StringDataName, UserIndex, Data);
		OnSaveStateChanged.Broadcast(ESaveState::LoadingComplete);
		bIsAsyncOperationInProgress = false;
	});

	bIsAsyncOperationInProgress = true;
	OnSaveStateChanged.Broadcast(ESaveState::Loading);
	UGameplayStatics::AsyncLoadGameFromSlot(DataName.ToString(), 0, LoadDelegate);
	return true;
}

bool ULimenSaveSubsystem::IsAsyncOperationInProgress() const
{
	return bIsAsyncOperationInProgress;
}
