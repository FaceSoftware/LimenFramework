// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "LimenMovementComponent.generated.h"

class FThreadedFunction;
class UCharacterMovementComponent;

UENUM(BlueprintType)
enum class ECustomMovementMode : uint8
{
	None				= 0x000000,
	CrouchWalking		= 0x000001,
	Sprinting			= 0x000002,
	CrouchSprinting		= 0x000003,
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMovementStatusUpdate);

/**
 * Class that communicates with a character movement component instance to support Sprint movement mode.
 */
UCLASS(Deprecated, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LIMENLOCOMOTION_API UDEPRECATED_LimenMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	static uint8 CustomMovementModeToByte(const ECustomMovementMode Mode)
	{
		return static_cast<uint8>(Mode);
	}
	
	UPROPERTY(BlueprintAssignable)
	FMovementStatusUpdate OnMovementStatusUpdated;
	
	UDEPRECATED_LimenMovementComponent();
	virtual void OnComponentCreated() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UFUNCTION(BlueprintCallable, Category="Limen|Gameplay")
	void ToggleSprint();
	/**
	 * @brief Sets the character's intent to sprint.
	 */
	UFUNCTION(BlueprintCallable, Category="Limen|Gameplay")
	void StartSprinting();
	/**
	 * @brief Removes the character's intent to sprint.
	 */
	UFUNCTION(BlueprintCallable, Category="Limen|Gameplay")
	void StopSprinting();
	/**
	 * @brief Checks if the character is sprinting.
	 * @return True if is sprinting. False if is not sprinting.
	 */
	UFUNCTION(BlueprintCallable, Category="Limen|Gameplay")
	bool IsSprinting() const;
	/**
	 * @brief Checks if the character wants to sprint.
	 * @return True if it wants. False if it does not.
	 */
	bool WantsToSprint() const;
	bool IsStill() const;

	ECustomMovementMode GetCustomMovementMode() const;

protected:
	virtual void SetMovementMode(EMovementMode NewMovementMode, uint8 NewCustomMode) override;
	virtual void UpdateMovementMode();
	
private:
	/**
	 * @brief The speed of the character's sprint.
	 */
	UPROPERTY(EditAnywhere, Category="Limen|Sprinting")
	float MaxSprintSpeed;
	UPROPERTY(EditAnywhere, Category="Limen|Crouch Sprinting")
	float MaxCrouchSprintSpeed;
	UPROPERTY(EditAnywhere, Category="Limen|Crouch Walking")
	float MaxCrouchWalkingSpeed;

	bool bWantsToSprint;

	/**
	 * @brief Defines the rules for the character to sprint.
	 * @return Whether the character can sprint or not.
	 */
	virtual bool CanSprint();
};