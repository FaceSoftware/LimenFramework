// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "LimenCharacterBase.generated.h"


class ALimenPlayerControllerBase;
class ULimenVariableMovementAbility;
class ULimenAbilityComponent;
class ALimenPlayerStateBase;
struct FNotificationParams;
struct FInputActionInstance;
class UInputAction;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCharacterCrouchDelegate, const bool, bIsCrouched);
DECLARE_MULTICAST_DELEGATE(FInputDelegate);

/**
 * Base class for the LimenCharacter, only includes locomotion
 * and is able to queue notifications if the controller class is the LimenPlayerController.
 */
UCLASS(Blueprintable)
class LIMENGAMEFRAMEWORK_API ALimenCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:	
	explicit ALimenCharacterBase(const FObjectInitializer& InObjectInitializer = FObjectInitializer::Get());
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void EnableInput(APlayerController* InPlayerController) override;
	virtual void DisableInput(APlayerController* InPlayerController) override;
	virtual bool CanJumpInternal_Implementation() const override;

	virtual void SetCinematicMode(bool bInCinematicMode, bool bHidePlayer, bool bAffectsHUD, bool bAffectsMovement, bool bAffectsTurning);
	
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category="Limen|Notifications")
	virtual bool QueueNotification(const FNotificationParams& InParams);
	
	/**
	 * @brief Getter for the player controller associated with this character.
	 * @return The player controller associated with this character.
	 */
	ALimenPlayerControllerBase* GetLimenBasePlayerController() const;
	
	UFUNCTION(BlueprintCallable, Category="Limen|Character")
	ULimenAbilityComponent* GetAbilityComponent() const;

	APlayerController* GetPlayerController() const;

	UFUNCTION(BlueprintCallable)
	FVector GetLookTarget(const float MaxDistance) const;

protected:
	UPROPERTY(EditAnywhere, Category="Limen", BlueprintReadOnly)
	TObjectPtr<ULimenAbilityComponent> AbilityComponent;

	ALimenPlayerStateBase* GetLimenBasePlayerState() const;
	UFUNCTION()
	virtual void SetupAbilityComponent(ULimenAbilityComponent* InAbilityComponent);
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SetCinematicMode(bool bInCinematicMode, bool bAffectsHUD, bool bAffectsMovement, bool bAffectsTurning);
	
private:
	UPROPERTY()
	TWeakObjectPtr<ALimenPlayerControllerBase> LimenBasePlayerController;
	UPROPERTY()
	TWeakObjectPtr<ALimenPlayerStateBase> LimenBasePlayerState;
	UPROPERTY()
	TWeakObjectPtr<APlayerController> PlayerController;

	void SetupAbilityComponentInternal(ULimenAbilityComponent* InAbilityComponent);
};
