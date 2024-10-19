// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LimenVariableMovementAbility.h"
#include "LimenHumanMovementAbility.generated.h"


class ULimenStepsSoundComponent;
class ULimenCameraShakeComponent;

/**
 * 
 */
UCLASS()
class LIMENABILITYSYSTEM_API ULimenHumanMovementAbility : public ULimenVariableMovementAbility
{
	GENERATED_BODY()
	
public:
	ULimenHumanMovementAbility();
	virtual void Initialize(AActor* InOwner) override;
	virtual void Tick(float DeltaTime) override;
	
private:
	TObjectPtr<ULimenCameraShakeComponent> CameraShakeComponent;
	TObjectPtr<UCharacterMovementComponent> CharacterMovementComponent;
	TObjectPtr<ULimenStepsSoundComponent> StepsSoundComponent;
	EMovementMode PreviousMovementMode;

	bool IsStill() const;

	void HandleCameraShake() const;
};
