// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
template<typename T>
struct TAtomicArray : protected TArray<T>
{
public:
	TAtomicArray() : TArray<T>()
	{
		
	}
	~TAtomicArray() {}
	
	int32 Insert(const T& Element)
	{
		FScopeLock Lock(&ArrayLock);
		return TArray<T>::Add(Element);
	}

	int32 Insert(T&& Element)
	{
		FScopeLock Lock(&ArrayLock);
		return TArray<T>::Add(MoveTemp(Element));
	}
	T PopAt(int32 Index)
	{
		FScopeLock Lock(&ArrayLock);
		T Element = TArray<T>::operator[](Index);
		TArray<T>::RemoveAt(Index, 1, EAllowShrinking::No);
		return Element;
	}
	void RemoveInvalid()
    {
    	FScopeLock Lock(&ArrayLock);
    	TArray<T>::Shrink();
    }
	TArray<T> Copy() const
	{
		FScopeLock Lock(&ArrayLock);
		return TArray<T>(static_cast<const TArray<T>&>(*this));
	}


	// Iterators (const only, to modify elements call PopAt and then Insert)

protected:
	mutable FCriticalSection ArrayLock;
};
