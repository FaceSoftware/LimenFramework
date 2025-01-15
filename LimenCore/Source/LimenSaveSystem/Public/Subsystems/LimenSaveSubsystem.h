// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "LimenSaveSubsystem.generated.h"

class USaveGame;
class ULimenSaveData;

UENUM(BlueprintType)
enum class ESaveState : uint8
{
	Undefined,
	Saving,
	SavingComplete,
	Loading,
	LoadingComplete,
};

UCLASS()
class LIMENSAVESYSTEM_API ULimenSaveSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAsyncSaveStateChanged, const ESaveState, CurrentSaveState);
	
public:	
	UPROPERTY(BlueprintAssignable)
	FAsyncSaveStateChanged OnSaveStateChanged;
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	static ULimenSaveSubsystem* Get(const UWorld* World);

	UFUNCTION(BlueprintCallable, Category="Limen|Save System")
	bool SaveData(ULimenSaveData* SaveData, const FString& DataName);
	
	bool SaveDataAsync(ULimenSaveData* SaveData, const FString& DataName, const TFunction<void(const FString&, const int32, const bool)>& SaveFinishCallback);
	
	UFUNCTION(BlueprintCallable, Category="Limen|Save System")
	ULimenSaveData* LoadData(const FString& DataName);

	template<typename SaveDataClass>
	SaveDataClass* LoadData(const FString& DataName)
	{
		static_assert(std::is_base_of_v<ULimenSaveData, SaveDataClass>);
		return Cast<SaveDataClass>(LoadData(DataName));
	}

	bool LoadDataAsync(const FName& DataName, const TFunction<void(const FString&, const int32, ULimenSaveData*)>& LoadFinishCallback);

	bool IsAsyncOperationInProgress() const;
	
protected:
	bool bIsAsyncOperationInProgress;
	
private:
	
	
};

class FLimenSerialization
{
public:
	static FName BoolToName(const bool bValue)
	{
		return bValue ? TEXT("1") : TEXT("0");
	}
	static bool NameToBool(const FName& bValue)
	{
		return bValue.ToString().ToBool();
	}
	
};
