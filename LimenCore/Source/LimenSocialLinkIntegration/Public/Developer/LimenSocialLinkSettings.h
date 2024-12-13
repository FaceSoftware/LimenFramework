// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "LimenSocialLinkSettings.generated.h"


USTRUCT(BlueprintType)
struct FSocialLink
{
	GENERATED_BODY()
	
public:
	FSocialLink();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Id;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UTexture2D> Image;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Text;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Hyperlink;
};

/**
 * 
 */
UCLASS(Config="Limen")
class LIMENSOCIALLINKINTEGRATION_API ULimenSocialLinkSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Config)
	TArray<FSocialLink> SocialLinks;

	UFUNCTION(BlueprintCallable)
	static bool GetSocialLink(const FName& Id, FSocialLink& OutLink);
	UFUNCTION(BlueprintCallable)
	static const TArray<FSocialLink>& GetSocialLinks();
	
	virtual FName GetContainerName() const override;
	virtual FName GetCategoryName() const override;
	virtual FName GetSectionName() const override;

#if WITH_EDITOR
	virtual FText GetSectionText() const override;
	virtual FText GetSectionDescription() const override;
#endif

};
