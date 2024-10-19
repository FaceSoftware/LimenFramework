// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/AudioComponent.h"
#include "DataAssets/LimenCharacterMovementAudioData.h"
#include "LimenStepsSoundComponent.generated.h"


class UAIPerceptionSystem;
enum class EHumanMovementSound : uint8;
class ULimenCharacterMovementAudioData;
class UPhysicalMaterial;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LIMENLOCOMOTION_API ULimenStepsSoundComponent : public UAudioComponent
{
	GENERATED_BODY()

public:
	ULimenStepsSoundComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
protected:
	void PlayStepAudio();
	virtual UPhysicalMaterial* GetStepPhysicalMaterial() const;
	virtual FVector2D GetStepAudioDelayAndVolumeLevelForVelocity(const double Velocity) const;
	virtual USoundBase* GetStepSoundToPlay(const UPhysicalMaterial* PhysicalMaterial);

private:
	UPROPERTY(EditDefaultsOnly, Category="Limen")
	TArray<TSoftObjectPtr<ULimenCharacterMovementAudioData>> SoundSettingsList;
	/**
	 * @brief Whether to play audio in the 3D world's space or ignore spacial audio.
	 */
	UPROPERTY(EditDefaultsOnly, Category="Limen")
	uint8 bPlayIn3dSpace : 1;
	UPROPERTY(EditDefaultsOnly, Category="Limen")
	TEnumAsByte<ECollisionChannel> LineTraceCollisionChannel;
	UPROPERTY(EditDefaultsOnly, Category="Limen", meta=(ClampMin="0"))
	double LineTraceTolerance;
	/**
	 * @brief The key should be the owner's velocity and the value should be a 2D Vector
	 * with X as the delay of the sound of the steps at that velocity and Y as the sound multiplier.
	 * It will interpolate linearly between the above / below index of the map.
	 */
	UPROPERTY(EditDefaultsOnly, Category="Limen")
	TMap<double, FVector2D> StepsSoundDelayBySpeed;

	/**
	 * @brief The speed which this component will start to register audio events with the AI system.
	 * Values below the specified will not register any audio events.
	 */
	UPROPERTY(EditDefaultsOnly, Category="Limen")
	double MinimumSilentSpeed;
	
	UPROPERTY()
	TObjectPtr<const ULimenCharacterMovementAudioData> CurrentAudioData;
	UPROPERTY()
	TObjectPtr<const UPhysicalMaterial> CurrentPhysicalMaterial;
	UPROPERTY()
	TObjectPtr<USoundBase> CurrentAudio;
	
	FTimerHandle StepsTimer;
	FCollisionQueryParams QueryParams;
	double PreviousVelocity;
	TObjectPtr<UPawnMovementComponent> OwnerMovement;
	TWeakObjectPtr<UAIPerceptionSystem> AIPerceptionSystem;

	// Cache
	FVector2D CachedAudioDelayAndVolumeLevel;
	UPROPERTY()
	TObjectPtr<USoundBase> CachedStepSound;
	
	void RegisterAudioEventForAI() const;
};
