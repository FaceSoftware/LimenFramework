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
	bool SaveData(ULimenSaveData* SaveData, const FName& DataName);
	
	bool SaveDataAsync(ULimenSaveData* SaveData, const FName& DataName, const TFunction<void(const FString&, const int32, const bool)>& SaveFinishCallback);
	
	UFUNCTION(BlueprintCallable, Category="Limen|Save System")
	ULimenSaveData* LoadData(const FName& DataName);

	template<typename SaveDataClass>
	SaveDataClass* LoadData(const FName& DataName)
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
