// Copyright © FaceSoftware. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "LevelSequence.h"
#include "LevelSequenceActor.h"
#include "LevelSequencePlayer.h"
#include "MovieSceneSequencePlaybackSettings.h"


template<typename... TArgs>
class TLimenTickable
{
	using FTickFunctorType = TFunction<bool(float, TArgs...)>;

public:
	TLimenTickable() : bLazyIsTicking(false)
	{
		
	}

	~TLimenTickable()
	{
		Stop();
	}

	void Start()
	{
		if (IsTicking()) return;

		Handle = FWorldDelegates::OnWorldTickEnd.AddLambda([this](UWorld* World, ELevelTick, float DeltaSeconds)
		{
			if (!Functor) return;

			const bool bDone = std::apply([this, DeltaSeconds] <typename... T0>(T0&&... UnpackedArgs)
				{
					return Functor(DeltaSeconds, Forward<T0>(UnpackedArgs)...);
				}
				, Args);

			if (bDone) Stop();
		});
	}

	void Stop()
	{
		if (!IsTicking()) { return; }

		FWorldDelegates::OnWorldTickEnd.Remove(Handle);
		Handle.Reset();
	}

	bool IsTicking() const
	{
		bLazyIsTicking = Handle.IsValid();
		return bLazyIsTicking;
	}

	TLimenTickable& operator=(const FTickFunctorType& InFunctor)
	{
		Stop();
		Functor = InFunctor;
		Start();
		return *this;
	}

private:
	FTickFunctorType Functor;
	TTuple<TDecay<TArgs>...> Args;
	FDelegateHandle Handle;
	mutable bool bLazyIsTicking;
};

namespace LimenLevelSequenceHelpers
{
	static ULevelSequencePlayer* CreateLevelSequencePlayerWithInstanceData(UObject* WorldContextObject, ULevelSequence* InLevelSequence, UObject* InInstanceData, FMovieSceneSequencePlaybackSettings Settings, ALevelSequenceActor*& OutActor)
	{
		if (InLevelSequence == nullptr)
		{
			return nullptr;
		}

		UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
		if (World == nullptr || World->bIsTearingDown)
		{
			return nullptr;
		}

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParams.ObjectFlags |= RF_Transient;
		SpawnParams.bAllowDuringConstructionScript = true;

		// Defer construction for autoplay so that BeginPlay() is called
		SpawnParams.bDeferConstruction = true;

		ALevelSequenceActor* Actor = World->SpawnActor<ALevelSequenceActor>(SpawnParams);

		Actor->PlaybackSettings = Settings;
		Actor->GetSequencePlayer()->SetPlaybackSettings(Settings);

		Actor->SetSequence(InLevelSequence);
		
		Actor->bOverrideInstanceData = true;
		Actor->DefaultInstanceData = InInstanceData;
	
		Actor->InitializePlayer();
	
		FTransform DefaultTransform;
		Actor->FinishSpawning(DefaultTransform);

		OutActor = Actor;

		return Actor->GetSequencePlayer();
	}
}
