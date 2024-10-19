// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "LimenCameraShakeComponent.generated.h"

/**
 * 
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LIMENLOCOMOTION_API ULimenCameraShakeComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	ULimenCameraShakeComponent();

	UFUNCTION(BlueprintCallable)
	void PlayStandCameraShake();
	UFUNCTION(BlueprintCallable)
	void PlayWalkCameraShake();
	UFUNCTION(BlueprintCallable)
	void PlaySprintCameraShake();
	UFUNCTION(BlueprintCallable)
	void StopCameraShake();
	UFUNCTION(BlueprintCallable)
	bool IsPlayingCameraShake() const;
	UFUNCTION(BlueprintCallable)
	bool IsPlayingStandCameraShake() const;
	UFUNCTION(BlueprintCallable)
	bool IsPlayingWalkCameraShake() const;
	UFUNCTION(BlueprintCallable)
	bool IsPlayingSprintCameraShake() const;
	
	virtual void SetActive(bool bNewActive, bool bReset) override;
	
protected:
	void PlayCameraShake();

private:
	UPROPERTY(EditDefaultsOnly, Category="Limen")
	TSubclassOf<UCameraShakeBase> StandCameraShake;
	
	UPROPERTY(EditDefaultsOnly, Category="Limen")
	TSubclassOf<UCameraShakeBase> WalkCameraShake;
	
	UPROPERTY(EditDefaultsOnly, Category="Limen")
	TSubclassOf<UCameraShakeBase> SprintCameraShake;

	UPROPERTY(EditDefaultsOnly, Category="Limen")
	ECameraShakePlaySpace CameraShakePlaySpace;

	UPROPERTY()
	TObjectPtr<UCameraShakeBase> ActiveCameraShake;
	UPROPERTY()
	TSubclassOf<UCameraShakeBase> ActiveCameraShakeClass;

	FTimerHandle CameraShakeTimer;
};
