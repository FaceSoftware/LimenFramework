// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LimenInteractableAreaComponent.h"
#include "LimenCinematicInteractableComponent.generated.h"


class ALimenGameplayActor;
class UCameraComponent;

/**
 * @brief Cinematic only work when a PlayerController interacts. For AIControllers this will behave as a normal.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LIMENINTERACTION_API ULimenCinematicInteractableComponent : public ULimenInteractableAreaComponent
{
	GENERATED_BODY()

public:
	DECLARE_MULTICAST_DELEGATE_TwoParams(FCinematicDelegate,
										 APlayerController* /* PlayerController */,
										 APawn* /* PlayerPawn */);

	FCinematicDelegate OnCinematicStart;
	FCinematicDelegate OnCinematicEnd;
	FCinematicDelegate OnCinematicTriggered;
	
	ULimenCinematicInteractableComponent();
	void AttachSubComponents();

	virtual void OnComponentCreated() override;
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif WITH_EDITOR

	UFUNCTION(BlueprintCallable)
	void AddItem(ALimenGameplayActor* Item);
	UFUNCTION(BlueprintCallable)
	void SetItemRelativeTransform(const FTransform& NewTransform);
	
	virtual void Interact(AController* InController, APawn* InPawn) override;

	UFUNCTION(BlueprintCallable)
	void NotifyInteract();

	UFUNCTION(BlueprintCallable)
	virtual void StartCinematic(APlayerController* InPlayerController, APawn* InPlayerPawn);
	UFUNCTION(BlueprintCallable)
	virtual void StopCinematic();

protected:
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UCameraComponent> CinematicCamera;
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<USceneComponent> ItemSocket;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FTransform CinematicCameraOffset;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FTransform ItemSocketTransform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FViewTargetTransitionParams TransitionParameters;

	APlayerController* GetCurrentPlayerController() const;
	APawn* GetCurrentPawn() const;
	ALimenGameplayActor* GetItemActor() const;

	virtual void OnCinematicStarted();
	virtual void OnCinematicStopped();
	
private:
	static const FAttachmentTransformRules CameraAttachmentRules;
	static const FAttachmentTransformRules ItemSocketAttachmentRules;
	
	UPROPERTY()
	TWeakObjectPtr<APlayerController> PlayerController;
	UPROPERTY()
	TWeakObjectPtr<APawn> PlayerPawn;
	UPROPERTY()
	TWeakObjectPtr<AActor> PlayerOriginalViewTarget;
	
	UPROPERTY()
	TWeakObjectPtr<ALimenGameplayActor> ItemActor;
};
