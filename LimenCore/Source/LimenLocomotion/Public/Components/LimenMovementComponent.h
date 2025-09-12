// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "LimenMovementComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMovementStatusUpdate);

/**
 * Class that communicates with a character movement component instance to support Sprint movement mode.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LIMENLOCOMOTION_API ULimenMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

	friend class FSavedMove_Limen;

	enum class EWalkModifier
	{
		None,
		SlowWalk,
		Sprint,
	};

public:
	explicit ULimenMovementComponent(const FObjectInitializer& InObjectInitializer);
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;

	void SetSprintMode(const bool bEnabled);
	bool IsSprinting() const;
	void SetSlowWalkMode(const bool bEnabled);
	bool IsSlowWalking() const;
	void SetWalkModifier(EWalkModifier NewModifier);
	EWalkModifier GetWalkModifier() const;

	void SetMaxSprintSpeed(const float NewSpeed);
	float GetMaxSprintSpeed() const;
	void SetCrouchSprintSpeed(const float NewSpeed);
	float GetCrouchSprintSpeed() const;

protected:
	UPROPERTY(EditDefaultsOnly, Category="Character Movement: Walking|Sprint")
	float SprintSpeed;
	UPROPERTY(EditDefaultsOnly, Category="Character Movement: Walking|Sprint")
	float CrouchSprintSpeed;
	UPROPERTY(EditDefaultsOnly, Category="Character Movement: Walking|Sprint", meta=(EditCondition="!bSlowWalkEnabledByDefault"))
	bool bSprintingEnabledByDefault;

	UPROPERTY(EditDefaultsOnly, Category="Character Movement: Walking|Slow Walk")
	float SlowWalkSpeed;
	UPROPERTY(EditDefaultsOnly, Category="Character Movement: Walking|Slow Walk")
	float CrouchSlowWalkSpeed;
	UPROPERTY(EditDefaultsOnly, Category="Character Movement: Walking|Slow Walk", meta=(EditCondition="!bSprintingEnabledByDefault"))
	bool bSlowWalkEnabledByDefault;

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

	virtual void OnWalkModeChanged(EWalkModifier NewMode);

private:
	EWalkModifier WalkMode;

	float MaxWalkSpeed_Cached;
	float MaxWalkSpeedCrouched_Cached;

	void SetupAirStrafing();
};

class FSavedMove_Limen : public FSavedMove_Character
{
public:
	using EWalkModifier = ULimenMovementComponent::EWalkModifier;
	EWalkModifier WalkModifier;

	FSavedMove_Limen()
	{
		WalkModifier = EWalkModifier::None;
	}

	virtual void Clear() override
	{
		FSavedMove_Character::Clear();
		WalkModifier = EWalkModifier::None;
	}

	virtual uint8 GetCompressedFlags() const override
	{
		/**
		 * FLAG_Custom_0		= 0x10, -> Sprint
		 * FLAG_Custom_1		= 0x20, -> SlowWalk
		 * FLAG_Custom_2		= 0x40,
		 * FLAG_Custom_3		= 0x80,
		 */
		uint8 Result = FSavedMove_Character::GetCompressedFlags();

		switch (WalkModifier)
		{
		case EWalkModifier::SlowWalk: Result |= FLAG_Custom_1; break;
		case EWalkModifier::Sprint: Result |= FLAG_Custom_0; break;
		default: break;
		}

		return Result;
	}

	virtual void SetMoveFor(ACharacter* C, const float InDeltaTime, FVector const& NewAccel,
							FNetworkPredictionData_Client_Character& ClientData) override
	{
		FSavedMove_Character::SetMoveFor(C, InDeltaTime, NewAccel, ClientData);

		if (const auto* Movement = Cast<ULimenMovementComponent>(C->GetCharacterMovement()))
		{
			WalkModifier = Movement->GetWalkModifier();
		}
	}

	virtual void PrepMoveFor(ACharacter* C) override
	{
		FSavedMove_Character::PrepMoveFor(C);

		if (auto* Movement = Cast<ULimenMovementComponent>(C->GetCharacterMovement()))
		{
			Movement->SetWalkModifier(WalkModifier);
		}
	}

	virtual bool CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter, float MaxDelta) const override
	{
		if (!FSavedMove_Character::CanCombineWith(NewMove, InCharacter, MaxDelta)) return false;

		const auto* NewLimenMove = static_cast<const FSavedMove_Limen*>(NewMove.Get());
		return WalkModifier == NewLimenMove->WalkModifier;
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
