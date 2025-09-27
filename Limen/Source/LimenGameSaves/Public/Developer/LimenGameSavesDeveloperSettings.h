// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "UMG/LimenGenericModalWidget.h"
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
	UPROPERTY(EditAnywhere, Config)
	FModalParams LoadGameFailedModalParams = FModalParams(TEXT("Save Data Corrupted"), TEXT("Something went wrong loading the save data."));
	UPROPERTY(EditAnywhere, Config)
	FModalParams SaveGameFailedModalParams = FModalParams(TEXT("Save Error"),TEXT("Something went wrong saving game data."));

	virtual FName GetContainerName() const override;
	virtual FName GetCategoryName() const override;
	virtual FName GetSectionName() const override;

#if WITH_EDITOR
	virtual FText GetSectionText() const override;
	virtual FText GetSectionDescription() const override;
#endif
};
