// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actors/LimenGameplayActor.h"
#include "Engine/Scene.h"
#include "Camera/PlayerCameraManager.h"
#include "LimenCinematicActor.generated.h"


class ALimenPlayerCharacter;
class UCameraComponent;

UCLASS(Abstract, Blueprintable)
class LIMENCINEMATICS_API ALimenCinematicActor : public ALimenGameplayActor
{
	GENERATED_BODY()

public:
	static const FName MainCameraComponentName;

	explicit ALimenCinematicActor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;

	virtual void StartCinematic(APlayerController* InPlayerController);
	virtual void StopCinematic();
	virtual void CancelCinematic();

	bool IsCinematicPlaying() const;

	AActor* GetOriginalViewTarget() const;

	const FPostProcessSettings& GetPostProcessSettings() const;
	void SetPostProcessSettings(const FPostProcessSettings& NewSettings);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UCameraComponent> CameraComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FViewTargetTransitionParams ViewTargetInTransitionParams;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FViewTargetTransitionParams ViewTargetOutTransitionParams;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="bOverrideCamerasPostProcessSettings"))
	FPostProcessSettings CinematicCamerasPostProcessSettings;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bOverrideCamerasPostProcessSettings;
	
	virtual void OnViewTargetSetToSelf();
	virtual void OnViewTargetRestored();

	UFUNCTION(BlueprintImplementableEvent)
	void CinematicStart();

private:
	TWeakObjectPtr<AActor> OriginalViewTarget;
	TWeakObjectPtr<APlayerController> CinematicContextPlayerController;
	
	FTimerHandle ViewTargetTransitionTimerHandle;
	bool bIsCinematicPlaying;

	UPROPERTY()
	TArray<UCameraComponent*> CinematicCameras;
	
	void OnViewTargetSetToSelf_Internal();
	void OnViewTargetRestored_Internal();
};
