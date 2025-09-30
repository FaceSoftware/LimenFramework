// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/LimenSaveObjectInterface.h"
#include "LimenSavesHandler.generated.h"

/**
 * @brief Handler that serializes gameplay actors for the save system.
 *
 * This class gathers all actors implementing ILimenSaveObjectInterface,
 * serializes their data (via FActorSaveData) and stores it locally.
 * The SaveGameSubsystem then serializes this handler into a USaveGame object.
 *
 * Key points:
 * - Uses recursive serialization: every UPROPERTY marked with SaveGame inside
 *   this handler (including arrays/maps/structs) will also be serialized.
 * - Only data marked with UPROPERTY(SaveGame) is persisted.
 * - Optional: classes don’t need to implement ILimenSaveObjectInterface,
 *   you can also pull specific data manually and store it here.
 *
 * Note: ILimenSaveObjectInterface methods are sealed (final) —
 * all handlers always allow saving/loading, and DataSaved/DataLoaded
 * callbacks are not meant to be overridden.
 */
UCLASS(Abstract)
class LIMENGAMESAVES_API ULimenSavesHandler : public UObject, public ILimenSaveObjectInterface
{
	GENERATED_BODY()

public:

#pragma region ILimenSaveObjectInterface

	virtual bool ShouldSaveData() const override;
	virtual bool ShouldLoadData() const override;
	virtual void PreDataSaved() override;
	virtual void PostDataSaved() override;
	virtual void PreDataLoaded() override;
	virtual void PostDataLoaded() override;

#pragma endregion

	virtual bool SaveDataFrom(UWorld* World);
	virtual bool LoadDataTo(UWorld* World);

	virtual UWorld* GetWorld() const override final;

private:
	TWeakObjectPtr<UWorld> WorldContext;

};
