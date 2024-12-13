// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "LimenCharacterBase.generated.h"


class ALimenPlayerControllerBase;
class ULimenVariableMovementAbility;
class ULimenAbilityComponent;
class ALimenBasePlayerState;
struct FNotificationParams;
struct FInputActionInstance;
class UInputAction;

USTRUCT(BlueprintType)
struct FMouseParameters
{
	GENERATED_BODY();
	
	float SensitivityX = 1;
	float SensitivityY = 1;
	bool bInvertAxisX = false;
	bool bInvertAxisY = false;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCharacterCrouchDelegate, const bool, bIsCrouched);
DECLARE_MULTICAST_DELEGATE(FInputDelegate);

/**
 * Base class for the LimenCharacter, only includes locomotion
 * and is able to queue notifications if the controller class is the LimenPlayerController.
 */
UCLASS(Abstract, NotBlueprintable)
class LIMENPLAYERS_API ALimenCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	FInputDelegate OnMovementInput;
	FInputDelegate OnSprintInput;
	
	explicit ALimenCharacterBase(const FObjectInitializer& InObjectInitializer = FObjectInitializer::Get());
	virtual void BeginPlay() override;

	virtual void EnableInput(APlayerController* InPlayerController) override;
	virtual void DisableInput(APlayerController* InPlayerController) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category="Limen|Notifications")
	virtual bool QueueNotification(const FNotificationParams& InParams);

	/**
	 * @brief The player will start sprinting if it can sprint.
	 */
	void StartSprinting();
	/**
	 * @brief The player will stop sprinting.
	 */
	void StopSprinting();
	/**
	 * @brief Toggles between StartSprinting or StopSprinting.
	 */
	void ToggleSprint();
	/**
	 * @brief Getter for the player controller associated with this character.
	 * @return The player controller associated with this character.
	 */
	ALimenPlayerControllerBase* GetLimenBasePlayerController() const;

	const FMouseParameters& GetMouseParameters() const;
	void SetMouseParameters(const FMouseParameters& InNewParams);
	
	UFUNCTION(BlueprintCallable, Category="Limen|Character")
	ULimenAbilityComponent* GetAbilityComponent() const;

	APlayerController* GetPlayerController() const;

	const TSoftObjectPtr<UInputAction>& GetMoveInputAction() const;
	const TSoftObjectPtr<UInputAction>& GetLookInputAction() const;
	const TSoftObjectPtr<UInputAction>& GetSprintInputAction() const;

	UFUNCTION(BlueprintCallable)
	FVector GetLookTarget(const float MaxDistance) const;
	
	
protected:
	// FVector 2D -> X axis = Move Forward/backwards, Y axis = Move Right/Left
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Limen|Input Actions")
	TSoftObjectPtr<UInputAction> MoveInputAction;
	// FVector 2D -> X axis = Look Up/Down, Y axis = Rotate Right/Left
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Limen|Input Actions")
	TSoftObjectPtr<UInputAction> LookInputAction;
	// bool -> true = Start, false = Un-crouch
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Limen|Input Actions")
	TSoftObjectPtr<UInputAction> SprintInputAction;
		
	UPROPERTY(EditAnywhere, Category="Limen|Input")
	FMouseParameters MouseParameters;
	
	UPROPERTY(EditAnywhere, Category="Limen")
	TObjectPtr<ULimenAbilityComponent> AbilityComponent;
	
	UFUNCTION()
	virtual void OnHealthAttributeEmpty(const float NewValue);
	
	virtual void MoveInput(const FInputActionInstance& Instance);
	virtual void LookInput(const FInputActionInstance& Instance);
	virtual void SprintInput(const FInputActionInstance& Instance);

	virtual void PossessedBy(AController* NewController) override;

	ALimenBasePlayerState* GetLimenBasePlayerState() const;
	
private:
	UPROPERTY()
	TWeakObjectPtr<ULimenVariableMovementAbility> VariableMovementAbility;
	UPROPERTY()
	TWeakObjectPtr<ALimenPlayerControllerBase> LimenBasePlayerController;
	UPROPERTY()
	TWeakObjectPtr<ALimenBasePlayerState> LimenBasePlayerState;
	UPROPERTY()
	TWeakObjectPtr<APlayerController> PlayerController;
};
