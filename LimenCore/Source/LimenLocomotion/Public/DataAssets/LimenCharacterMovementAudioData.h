// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LimenCharacterMovementAudioData.generated.h"


/**
 * 
 */
UCLASS(Blueprintable)
class LIMENLOCOMOTION_API ULimenCharacterMovementAudioData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category="Limen")
	TSoftObjectPtr<UPhysicalMaterial> PhysicalMaterial;

	UPROPERTY(EditAnywhere, Category="Limen")
	TSoftObjectPtr<USoundBase> StepsAudio;
	
	UPROPERTY(EditAnywhere, Category="Limen")
	TSoftObjectPtr<USoundAttenuation> Attenuation;
};
