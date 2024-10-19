// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LimenStorageSubsystem.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "LimenArchiveSubsystem.generated.h"

class ULimenArchive;
/**
 * 
 */
UCLASS()
class LIMENARCHIVES_API ULimenArchiveSubsystem : public ULimenStorageSubsystem
{
	GENERATED_BODY()

public:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION(BlueprintCallable, Category="Limen|Archives")
	void AddArchive(ULimenArchive* NewArchive);

protected:

private:
	TSubclassOf<ULimenArchive> CurrentArchiveClass;
	
	virtual void Load_Internal() override;
};
