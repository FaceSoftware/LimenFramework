// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "LimenSaveSubsystem.generated.h"

class ULimenSaveWidget;
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

	ULimenSaveSubsystem();
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	static ULimenSaveSubsystem* Get(const UWorld* World);

	UFUNCTION(BlueprintCallable, Category="Limen|Save System")
	bool SaveData(ULimenSaveData* SaveData, const FString& DataName);
	template<typename T>
	bool SaveData(const FString& DataName)
	{
		static_assert(TIsDerivedFrom<T, ULimenSaveData>::IsDerived);
		return SaveData(NewObject<T>(), DataName);
	}
	
	bool SaveDataAsync(ULimenSaveData* SaveData, const FString& DataName, const TFunction<void(const FString&, const int32, const bool)>& SaveFinishCallback);
	template<typename T>
	bool SaveDataAsync(const FString& DataName, const TFunction<void(const FString&, const int32, const bool)>& SaveFinishCallback)
	{
		static_assert(TIsDerivedFrom<T, ULimenSaveData>::IsDerived);
		return SaveDataAsync(NewObject<T>(), DataName, SaveFinishCallback);
	}
	
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
	void AsyncSaveStarted(const FString& StringDataName, const int32 UserIndex);
	void AsyncSaveFinished(const FString& StringDataName, const int32 UserIndex, const bool bSuccess);

private:
	bool bIsAsyncOperationInProgress;

	bool bShouldShowSaveWidget;
	TStrongObjectPtr<ULimenSaveWidget> SaveWidgetStrongPtr;

	UFUNCTION()
	void SaveWidgetAnimationFinished(const bool bIsVisible);
};

class FLimenSerialization
{
public:
	FORCEINLINE static FName BoolToName(const bool bValue)
	{
		return bValue ? TEXT("1") : TEXT("0");
	}
	FORCEINLINE static bool NameToBool(const FName& Value)
	{
		return Value.ToString().ToBool();
	}
};
