// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actors/LimenGameplayManager.h"
#include "Interfaces/LimenSaveObjectInterface.h"
#include "LimenObjective.generated.h"

class ULimenObjectiveDataAsset;
class ALimenBaseCharacter;
class ALimenBasePlayerController;

UENUM(BlueprintType)
enum class EObjectiveState : uint8
{
	NotStarted,
	InProgress,
	Completed,
	Impossible,
};

USTRUCT(BlueprintType)
struct LIMENOBJECTIVES_API FObjectiveData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, SaveGame)	
	FName UniqueName;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, SaveGame)
	FText Title;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, SaveGame)
	FText Description;

	UPROPERTY(BlueprintReadWrite, SaveGame)
	EObjectiveState State = EObjectiveState::NotStarted;

	UPROPERTY(BlueprintReadWrite, SaveGame)
	float CurrentProgress = 0.f;

	UPROPERTY(BlueprintReadWrite, SaveGame)
	float MaxProgress = 0.f;

	void operator=(const FObjectiveData& Other);
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FObjectiveDataDelegate, ALimenObjective*, Objective, const FObjectiveData&, NewData);

UCLASS(Abstract, Blueprintable)
class LIMENOBJECTIVES_API ALimenObjective : public ALimenGameplayManager, public ILimenSaveObjectInterface
{
	GENERATED_BODY()

public:	
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category="Limen|Objectives")
	FObjectiveDataDelegate OnObjectiveUpdated;

	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category="Limen|Objectives")
	FObjectiveDataDelegate OnObjectiveAdded;

	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category="Limen|Objectives")
	FObjectiveDataDelegate OnObjectiveCompleted;

	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category="Limen|Objectives")
	FObjectiveDataDelegate OnObjectiveLoaded;

	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category="Limen|Objectives")
	FObjectiveDataDelegate OnObjectiveReset;

	ALimenObjective();
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Limen|Objectives")
	void BindToPlayer(APlayerController* PlayerController, APawn* Pawn);
	void BindToCurrentPlayer();

	UFUNCTION(BlueprintCallable, Category="Limen|Objectives")
	const FObjectiveData& GetObjectiveData() const;

	UFUNCTION(BlueprintCallable, Category="Limen|Objectives")
	APlayerController* GetBoundPlayerController() const;
	
	UFUNCTION(BlueprintCallable, Category="Limen|Objectives")
	APawn* GetBoundPawn() const;

	void LoadObjectiveForPlayer(APlayerController* PlayerController, APawn* Pawn, const FObjectiveData& NewData);
	
	UFUNCTION(BlueprintCallable, Category="Limen|Objectives")
	bool IsCompleted() const;

	UFUNCTION(BlueprintCallable, Category="Limen|Objectives")
	const FObjectiveData& GetData() const;

	void ResetObjective();

#pragma region ILimenSaveObjectInterface

	virtual bool ShouldSaveData() const override;
	virtual bool ShouldLoadData() const override;
	virtual void PreDataSaved() override;
	virtual void PostDataSaved() override;
	virtual void PreDataLoaded() override;
	virtual void PostDataLoaded() override;

#pragma endregion
	
protected:
	UPROPERTY(EditDefaultsOnly, Category="Limen")
	FName UniqueName;
	
	UPROPERTY(EditDefaultsOnly, Category="Limen")
	FText Title;

	UPROPERTY(EditDefaultsOnly, Category="Limen")
	FText Description;
	
	UFUNCTION(BlueprintCallable, Category="Limen|Objectives")
	void CompleteObjective();

	UFUNCTION(BlueprintNativeEvent, Category="Limen|Objectives")
	void ObjectiveDataChanged(const FObjectiveData& NewData);
	virtual void ObjectiveDataChanged_Implementation(const FObjectiveData& NewData) {}
	
	UFUNCTION(BlueprintNativeEvent, Category="Limen|Objectives")
	void ObjectiveStarted(APlayerController* PlayerController, APawn* Pawn);
	virtual void ObjectiveStarted_Implementation(APlayerController* PlayerController, APawn* Pawn) {}

	UFUNCTION(BlueprintNativeEvent, Category="Limen|Objectives")
	void ObjectiveCompleted(APlayerController* PlayerController, APawn* Pawn);
	virtual void ObjectiveCompleted_Implementation(APlayerController* PlayerController, APawn* Pawn) {}

	virtual void Start() override;
	
private:	
	UPROPERTY()
	TObjectPtr<APlayerController> BoundPlayerController;

	UPROPERTY()
	TObjectPtr<APawn> BoundPawn;

	UPROPERTY(SaveGame)
	FObjectiveData ObjectiveData;
};
