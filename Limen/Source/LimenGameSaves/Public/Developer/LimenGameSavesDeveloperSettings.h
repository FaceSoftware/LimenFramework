// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "LimenGameSavesDeveloperSettings.generated.h"

class ULimenSavesHandler;
/**
 * 
 */
UCLASS(Config="Limen")
class LIMENGAMESAVES_API ULimenGameSavesDeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Config)
	TArray<TSoftClassPtr<ULimenSavesHandler>> SaveHandlers;

	virtual FName GetContainerName() const override;
	virtual FName GetCategoryName() const override;
	virtual FName GetSectionName() const override;

#if WITH_EDITOR
	virtual FText GetSectionText() const override;
	virtual FText GetSectionDescription() const override;
#endif
};
