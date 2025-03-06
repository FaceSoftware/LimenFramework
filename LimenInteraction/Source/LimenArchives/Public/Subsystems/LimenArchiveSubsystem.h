// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LimenStorageSubsystem.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Templates/SubclassOf.h"
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
	ULimenArchiveSubsystem();
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

	UFUNCTION(BlueprintCallable, Category="Limen|Archives")
	void AddArchive(ULimenArchive* NewArchive);

protected:

private:	
	virtual void Load_Internal() override;
};
