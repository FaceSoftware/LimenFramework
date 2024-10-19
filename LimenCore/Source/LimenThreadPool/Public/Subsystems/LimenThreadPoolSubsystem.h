// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "LimenThreadPoolSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class LIMENTHREADPOOL_API ULimenThreadPoolSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	friend class FThreadPoolSpec;
	
	class FPoolWorker : public FRunnable
	{		
	public:
		FString ThreadName;
		
		FPoolWorker();
		virtual ~FPoolWorker() override;
		virtual void Exit() override;
		virtual bool Init() override;
		virtual uint32 Run() override;
		virtual void Stop() override;

		void QueueJob(const TFunction<void()>& Function);

		int32 GetQueuedJobsCount();
		void DiscardWaitEvent();
		
	private:
		bool bShouldStop;
		FCriticalSection QueueSection;
		TSharedPtr<FEvent> QueueCondition;
		TArray<TFunction<void()>> QueuedJobs;
	};

public:
	ULimenThreadPoolSubsystem();
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	void AddJob(const TFunction<void()>& Function) const;

protected:
	void CreateThreads();
	void DestroyThreads();
	TSharedRef<FPoolWorker, ESPMode::NotThreadSafe> GetAvailableThread() const;

private:
	TMap<TSharedRef<FPoolWorker, ESPMode::NotThreadSafe>, TSharedRef<FRunnableThread, ESPMode::NotThreadSafe>> ThreadPool;
};

