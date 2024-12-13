// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "LimenCharacterBase.generated.h"


class ALimenPlayerControllerBase;
class ULimenHealthAttribute;
class ULimenVariableMovementAbility;
class ULimenAbilityComponent;
class ULimenMovementComponent;
class ALimenBasePlayerState;
struct FItemData;
struct FNotificationParams;
struct FObjectiveData;
struct FInputActionInstance;
class ULimenObjective;
class UInputAction;
class UCameraComponent;
class UInputMappingContext;
class ULimenSprintComponent;

USTRUCT(BlueprintType)
struct FMouseParameters
{
	GENERATED_BODY();
	
	float SensitivityX = 1;
	float SensitivityY = 1;
	bool bInvertAxisX = false;
	bool bInvertAxisY = false;
};

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
	explicit ALimenCharacterBase(const FObjectInitializer& InObjectInitializer = FObjectInitializer::Get());
	
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category="Limen|Notifications")
	virtual bool QueueNotification(const FNotificationParams& InParams);
	
	ALimenPlayerControllerBase* GetLimenBasePlayerController() const;

	const FMouseParameters& GetMouseParameters() const;
	void SetMouseParameters(const FMouseParameters& InNewParams);

	APlayerController* GetPlayerController() const;

	const TSoftObjectPtr<UInputAction>& GetMoveInputAction() const;
	const TSoftObjectPtr<UInputAction>& GetLookInputAction() const;
	const TSoftObjectPtr<UInputAction>& GetSprintInputAction() const;
	
	
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
	
	virtual void MoveInput(const FInputActionInstance& Instance);
	virtual void LookInput(const FInputActionInstance& Instance);
	
	virtual void PossessedBy(AController* NewController) override;
	
private:
	UPROPERTY()
	TWeakObjectPtr<ALimenPlayerControllerBase> LimenBasePlayerController;
	UPROPERTY()
	TWeakObjectPtr<APlayerController> PlayerController;
};
