// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class FThreadingStatics
{
public:
	/**
	 * @brief Creates a thread for a runnable.
	 * @param InRunnable 
	 * @param ThreadName 
	 * @param Priority 
	 * @param InStackSize 
	 * @return 
	 */
	static TUniquePtr<FRunnableThread> MakeRunnableThread(const TSharedRef<FRunnable>& InRunnable, const TCHAR* ThreadName, EThreadPriority Priority = TPri_Normal, uint32 InStackSize = 0)
	{
		auto* Thread = FRunnableThread::Create(InRunnable.ToSharedPtr().Get(), ThreadName, InStackSize, Priority);
		return TUniquePtr<FRunnableThread>(Thread);
	}
};
