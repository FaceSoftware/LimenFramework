// Copyright © 2024 FaceSoftware. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "Templates/SubclassOf.h"
#include "LimenOnlineDeveloperSettings.generated.h"

class ULimenChatBox;
/**
 * 
 */
UCLASS(Config=Limen)
class LIMENONLINECHAT_API ULimenOnlineDeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	ULimenOnlineDeveloperSettings() = default;
	
	UPROPERTY(EditAnywhere, Config)
	FGuid ChatEncryptionKey;
	
	
	virtual FName GetContainerName() const override
	{
		return TEXT("Project");
	}
	virtual FName GetCategoryName() const override
	{
		return TEXT("Project");
	}
	virtual FName GetSectionName() const override
	{
		return TEXT("Limen Online");
	}

#if WITH_EDITOR
	
	virtual FText GetSectionText() const override
	{
		return FText::FromString(TEXT("Limen Online"));
	}
	virtual FText GetSectionDescription() const override
	{
		return FText::FromString(TEXT("Limen Online settings"));
	}
	
#endif
};
