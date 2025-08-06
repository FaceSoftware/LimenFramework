// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/SoftObjectPtr.h"
#include "LimenSoftObjectPtr.generated.h"


/**
 * 
 */
USTRUCT(BlueprintType)
struct LIMENCORE_API FLimenSoftObjectPtr
{
	GENERATED_BODY()

public:
	void LoadAsync()
	{
		LoadAsync(TFunction<void(UObject*)>());
	}
	template<typename T>
	void LoadAsync(const TFunction<void(T*)>& Callback)
	{
		if (SoftPtr.IsNull()) return;
		
		SoftPtr.LoadAsync(FLoadSoftObjectPathAsyncDelegate::CreateLambda([this, Callback]
			(const FSoftObjectPath&, UObject* Ptr)
		{
			T* LoadedObject = Cast<T>(Ptr);
			if (Callback.IsSet()) Callback(LoadedObject);
			StrongPtr = TStrongObjectPtr(LoadedObject);
		}));
	}
	template<typename T>
	T* LoadSynchronous()
	{
		StrongPtr = TStrongObjectPtr(SoftPtr.LoadSynchronous());
		return Cast<T>(StrongPtr.Get());
	}
	void LoadSynchronous()
	{
		StrongPtr = TStrongObjectPtr(SoftPtr.LoadSynchronous());
	}
	void ReleaseHardRef()
	{
		StrongPtr.Reset();
	}
	
	template<typename T>
	operator T()
	{
		return Cast<T>(StrongPtr.Get());
	}
	template<typename T>
	T* Get()
	{
		return Cast<T>(StrongPtr.Get());
	}
	bool IsValid() const
	{
		return StrongPtr.IsValid();
	}
	bool IsNull() const
	{
		return SoftPtr.IsNull();
	}

	TSoftObjectPtr<UObject> SoftObjectPtr() const
	{
		return SoftPtr;
	}

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UObject> SoftPtr;

	TStrongObjectPtr<UObject> StrongPtr;
};
