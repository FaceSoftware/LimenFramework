// Copyright © FaceSoftware. All rights reserved.


#include "Components/LimenFMODAudioComponent.h"

#include "GameMode/LimenGameModeBase.h"


ULimenFMODAudioComponent::ULimenFMODAudioComponent()
{
	bPausePlaybackWithWorld = true;
	bAutoActivate = false;
}

void ULimenFMODAudioComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwner()->HasAuthority())
	{
		GetWorld()->GetAuthGameMode<ALimenGameModeBase>()->OnGamePauseStateChanged.AddUniqueDynamic(this, &ThisClass::GamePausedStateChanged);
	}
}

void ULimenFMODAudioComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorld()->GetAuthGameMode<ALimenGameModeBase>()->OnGamePauseStateChanged.RemoveDynamic(this, &ThisClass::GamePausedStateChanged);

	Super::EndPlay(EndPlayReason);
}

void ULimenFMODAudioComponent::SetPausePlaybackWithWorld(const bool bPause)
{
	bPausePlaybackWithWorld = bPause;
}

void ULimenFMODAudioComponent::GamePausedStateChanged(const bool bIsPaused)
{
	if (bPausePlaybackWithWorld)
	{
		SetPaused(bIsPaused);
	}
}

ALimenFMODAmbientSound::ALimenFMODAmbientSound(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass(TEXT("FMODAudioComponent0"), ULimenFMODAudioComponent::StaticClass()))
{
	
}
