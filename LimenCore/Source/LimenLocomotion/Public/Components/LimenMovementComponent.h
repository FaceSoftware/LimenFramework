// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "LimenMovementComponent.generated.h"

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
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LIMENLOCOMOTION_API ULimenMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

	friend class FSavedMove_Limen;

public:
	explicit ULimenMovementComponent(const FObjectInitializer& InObjectInitializer);
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;

	void SetFastMovement(const bool bEnabled);
	bool IsFastMovementEnabled() const;

protected:
	UPROPERTY(EditDefaultsOnly, Category="Character Movement: Fast Movement")
	float FastWalkSpeedMultiplier;
	UPROPERTY(EditDefaultsOnly, Category="Character Movement: Fast Movement")
	float CrouchFastWalkSpeedMultiplier;
	UPROPERTY(EditDefaultsOnly, Category="Character Movement: Fast Movement")
	bool bFastMovementEnabledByDefault;
	UPROPERTY(EditDefaultsOnly, Category="Character Movement: Air Strafing")
	bool bEnableAirStrafing;
	UPROPERTY(EditDefaultsOnly, Category="Character Movement: Air Strafing")
	float AirAcceleration;
	UPROPERTY(EditDefaultsOnly, Category="Character Movement: Air Strafing")
	float MaxAirStrafeSpeed;
	UPROPERTY(EditDefaultsOnly, Category="Character Movement: Air Strafing")
	bool bLogCurrentSpeed;
	UPROPERTY(EditDefaultsOnly, Category="Character Movement: Jumping / Falling")
	bool bAllowJumpingWhileCrouched;

	virtual void UpdateFromCompressedFlags(uint8 Flags) override;
	virtual void PhysFalling(float DeltaTime, int32 Iterations) override;
	virtual void PhysAirStrafing(float DeltaTime);
	virtual bool CanAttemptJump() const override;

	virtual void OnFastMovementChanged();

private:
	bool bIsFastMovementEnabled;

	void SetupAirStrafing();
};

class FSavedMove_Limen : public FSavedMove_Character
{
public:
	bool bSavedIsFastMovementEnabled;

	FSavedMove_Limen()
	{
		bSavedIsFastMovementEnabled = false;
	}

	virtual void Clear() override
	{
		FSavedMove_Character::Clear();
		bSavedIsFastMovementEnabled = false;
	}

	virtual uint8 GetCompressedFlags() const override
	{
		/**
		 * FLAG_Custom_0		= 0x10, -> Fast Movement
		 * FLAG_Custom_1		= 0x20,
		 * FLAG_Custom_2		= 0x40,
		 * FLAG_Custom_3		= 0x80,
		 */
		uint8 Result = FSavedMove_Character::GetCompressedFlags();
		if (bSavedIsFastMovementEnabled)
		{
			Result |= FLAG_Custom_0;
		}
		return Result;
	}

	virtual void SetMoveFor(ACharacter* C, const float InDeltaTime, FVector const& NewAccel,
							FNetworkPredictionData_Client_Character& ClientData) override
	{
		FSavedMove_Character::SetMoveFor(C, InDeltaTime, NewAccel, ClientData);

		if (const auto* Movement = Cast<ULimenMovementComponent>(C->GetCharacterMovement()))
		{
			bSavedIsFastMovementEnabled = Movement->IsFastMovementEnabled();
		}
	}

	virtual void PrepMoveFor(ACharacter* C) override
	{
		FSavedMove_Character::PrepMoveFor(C);

		if (auto* Movement = Cast<ULimenMovementComponent>(C->GetCharacterMovement()))
		{
			Movement->SetFastMovement(bSavedIsFastMovementEnabled);
		}
	}
};


class FNetworkPredictionData_Client_Limen : public FNetworkPredictionData_Client_Character
{
public:
	explicit FNetworkPredictionData_Client_Limen(const UCharacterMovementComponent& ClientMovement) :
		FNetworkPredictionData_Client_Character(ClientMovement)
	{
	}

	virtual FSavedMovePtr AllocateNewMove() override
	{
		return MakeShared<FSavedMove_Limen>();
	}
};
