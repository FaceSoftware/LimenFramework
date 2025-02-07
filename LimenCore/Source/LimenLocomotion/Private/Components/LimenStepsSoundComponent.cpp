// Copyright Face Software. All Rights Reserved.


#include "Components/LimenStepsSoundComponent.h"

#include "TimerManager.h"
#include "DataAssets/LimenCharacterMovementAudioData.h"
#include "Engine/HitResult.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Perception/AIPerceptionSystem.h"
#include "Perception/AISense_Hearing.h"
#include "PhysicalMaterials/PhysicalMaterial.h"


ULimenStepsSoundComponent::ULimenStepsSoundComponent() : Super()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bRunOnAnyThread = false;
	PrimaryComponentTick.TickInterval = 1 / 32;
	bDisableParameterUpdatesWhilePlaying = true;

	bPlayIn3dSpace = true;
	LineTraceTolerance = 10.0;
	Sound = nullptr;
	LineTraceCollisionChannel = ECC_Visibility;
	MinimumSilentSpeed = 400;
	PreviousVelocity = 0;
	bCanPlayMultipleInstances = true;
	CachedAudioDelayAndVolumeLevel = FVector2D();
}

void ULimenStepsSoundComponent::BeginPlay()
{
	Super::BeginPlay();

	QueryParams.AddIgnoredActor(GetOwner());
	QueryParams.bReturnPhysicalMaterial = true;

	OwnerMovement = GetOwner()->GetComponentByClass<UPawnMovementComponent>();

	UAISystem* AISystem = Cast<UAISystem>(GetWorld()->GetAISystem());
	if (AISystem != nullptr)
	{
		AIPerceptionSystem = AISystem->GetPerceptionSystem();
	}
}

void ULimenStepsSoundComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	const double CurrentVelocity = OwnerMovement->Velocity.Length();
	const FVector2D SoundDelayAndVolume = GetStepAudioDelayAndVolumeLevelForVelocity(CurrentVelocity);
	const UPhysicalMaterial* PhysicalMaterial = GetStepPhysicalMaterial();
	USoundBase* SoundToPlay = PhysicalMaterial == nullptr ? nullptr : GetStepSoundToPlay(PhysicalMaterial);

	if (!FMath::IsNearlyZero(CurrentVelocity) && !FMath::IsNearlyEqualByULP(CurrentVelocity, PreviousVelocity, 0))
	{
		SetVolumeMultiplier(SoundDelayAndVolume.Y);
		if (!GetWorld()->GetTimerManager().IsTimerActive(StepsTimer))
		{
			GetWorld()->GetTimerManager().SetTimer(StepsTimer, this, &ThisClass::PlayStepAudio, SoundDelayAndVolume.X, false);
		}
		
		CachedAudioDelayAndVolumeLevel.X = SoundDelayAndVolume.X;
		CachedAudioDelayAndVolumeLevel.Y = SoundDelayAndVolume.Y;
		CurrentPhysicalMaterial = PhysicalMaterial;
		CurrentAudio = SoundToPlay;
		
		PreviousVelocity = CurrentVelocity;
	}
}

void ULimenStepsSoundComponent::PlayStepAudio()
{
	Sound = CurrentAudio;
	Play(0);
	if (OwnerMovement->Velocity.Length() >= MinimumSilentSpeed)
	{
		RegisterAudioEventForAI();
	}
}

UPhysicalMaterial* ULimenStepsSoundComponent::GetStepPhysicalMaterial() const
{
	FVector Origin, BoxExtent;
	GetOwner()->GetActorBounds(true, Origin, BoxExtent);
	
	FHitResult OutHit;
	const FVector End = Origin + FVector(0.0, 0.0, -BoxExtent.Z - LineTraceTolerance);
	GetWorld()->LineTraceSingleByChannel(OutHit, Origin, End, LineTraceCollisionChannel, QueryParams);

	return OutHit.PhysMaterial.Get();
}

FVector2D ULimenStepsSoundComponent::GetStepAudioDelayAndVolumeLevelForVelocity(const double Velocity) const
{
	if (StepsSoundDelayBySpeed.IsEmpty())
	{
		return FVector2D::Zero();
	}
	
	// Find the closest lower and upper velocities in the StepsSoundDelayBySpeed map
	double LowerVelocity = 0.0;
	double UpperVelocity = 0.0;
	bool bFoundUpperVelocity = false;
	for (const auto& Elem : StepsSoundDelayBySpeed)
	{
		if (Elem.Key <= Velocity)
		{
			LowerVelocity = Elem.Key;
		}
		else
		{
			UpperVelocity = Elem.Key;
			bFoundUpperVelocity = true;
			break;
		}
	}

	// If there's no upper velocity, return the value for the maximum velocity in the map
	if (!bFoundUpperVelocity)
	{
		double MaxKey = 0.0;
		for (const auto& Elem : StepsSoundDelayBySpeed)
		{
			if (Elem.Key > MaxKey)
			{
				MaxKey = Elem.Key;
			}
		}
		return StepsSoundDelayBySpeed[MaxKey];
	}

	// Interpolate between the lower and upper velocities
	const FVector2D& LowerValue = StepsSoundDelayBySpeed[LowerVelocity];
	const FVector2D& UpperValue = StepsSoundDelayBySpeed[UpperVelocity];
	const double Alpha = (Velocity - LowerVelocity) / (UpperVelocity - LowerVelocity);

	// Linear interpolation
	return FMath::Lerp(LowerValue, UpperValue, Alpha);
}

USoundBase* ULimenStepsSoundComponent::GetStepSoundToPlay(const UPhysicalMaterial* PhysicalMaterial)
{
	check(PhysicalMaterial != nullptr);

#if WITH_EDITOR

	for (auto& Element : SoundSettingsList)
	{
		ensureAlways(!Element.IsNull());
	}

#endif
	
	for (const TSoftObjectPtr<ULimenCharacterMovementAudioData>& Data : SoundSettingsList)
	{
		if (Data.IsNull())
		{
			continue;
		}

		if (Data.LoadSynchronous()->PhysicalMaterial == PhysicalMaterial)
		{
			CurrentAudioData = Data.LoadSynchronous();
			return Data->StepsAudio.LoadSynchronous();
		}
	}
	
	return nullptr;
}

void ULimenStepsSoundComponent::RegisterAudioEventForAI() const
{
	if (AIPerceptionSystem != nullptr)
	{
		return;
	}
	
	float Distance = 1000;
	if (CurrentAudioData != nullptr && CurrentAudioData->Attenuation != nullptr)
	{
		Distance = CurrentAudioData->Attenuation->Attenuation.FalloffDistance;
	}
	const FAINoiseEvent Event(GetOwner(), GetComponentLocation(), 1, Distance);
	AIPerceptionSystem->OnEvent(Event);
}
