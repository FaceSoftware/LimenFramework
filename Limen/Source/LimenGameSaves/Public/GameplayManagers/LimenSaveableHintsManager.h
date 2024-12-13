// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayManagers/LimenHintsManager.h"
#include "Interfaces/LimenSaveObjectInterface.h"
#include "LimenSaveableHintsManager.generated.h"

UCLASS()
class LIMENGAMESAVES_API ALimenSaveableHintsManager : public ALimenHintsManager, public ILimenSaveObjectInterface
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	virtual bool ShouldLoadData() const override;
	virtual bool ShouldSaveData() const override;
	virtual void DataLoaded() override;
	virtual void DataSaved() override;

private:
	UPROPERTY(SaveGame)
	TArray<FSoftClassPath> SavedHintClasses;

	void LoadSavedHints();
	void SaveHints();
	void SetupRemoveStaleHints();
};
