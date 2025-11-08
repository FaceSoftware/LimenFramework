// Copyright © FaceSoftware. All rights reserved.

#pragma once

#include "CoreMinimal.h"


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
