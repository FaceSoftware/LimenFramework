// Copyright © 2024 FaceSoftware. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Containers/SpscQueue.h"
#include "HAL/Event.h"
#include "HAL/Runnable.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "LimenThreadPoolSubsystem.generated.h"


class FEventRef;

/**
 * 
 */
UCLASS()
class LIMENTHREADPOOL_API ULimenThreadPoolSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	friend class FThreadPoolSpec;

public:
	struct ISliceJob
	{
		virtual ~ISliceJob() = default;

		// Do work for up to BudgetSeconds. Return true if finished.
		virtual bool ExecuteSlice(double BudgetSeconds) = 0;

		struct FBudgetHelper
		{
			explicit FBudgetHelper(const double BudgetSeconds)
			{
				Start = FPlatformTime::Cycles64();
				Budget = FPlatformTime::SecondsToCycles64(BudgetSeconds);
			}

			bool ShouldYield() const
			{
				return FPlatformTime::Cycles64() - Start >= Budget;
			}

		private:
			uint64 Start;
			uint64 Budget;
		};
	};

private:
	struct FDummySliceJob final : ISliceJob
	{
		explicit FDummySliceJob(const TFunction<bool(double)>& InJob) : Job(InJob)
		{
			
		}
		virtual bool ExecuteSlice(const double BudgetSeconds) override
		{
			Job.CheckCallable();
			return Job(BudgetSeconds);
		}

	private:
		TFunction<bool(double)> Job;
	};
	struct FUnslicedJob final : ISliceJob
	{
		explicit FUnslicedJob(const TFunction<void()>& InJob) : Job(InJob)
		{
			
		}
		virtual bool ExecuteSlice(const double BudgetSeconds) override
		{
			Job.CheckCallable();
			Job();
			return true;
		}

	private:
		TFunction<void()> Job;
	};
	class FPoolWorker final : public FRunnable
	{		
	public:
		FString ThreadName;
		
		FPoolWorker();
		virtual ~FPoolWorker() override;
		virtual void Exit() override;
		virtual bool Init() override;
		virtual uint32 Run() override;
		virtual void Stop() override;

		void QueueJob(const TSharedRef<ISliceJob>& NewJob);

		int32 GetQueuedJobsCount() const;
		void DiscardWaitEvent() const;
		
	private:
		std::atomic<bool> bShouldStop;
		FSharedEventRef QueueCondition;
		TSpscQueue<TSharedPtr<ISliceJob>> QueuedJobs;
		std::atomic_int32_t QueuedJobsCount;
		const double SliceBudget = 1.f / 16.f; // 16 frame times
	};

public:
	ULimenThreadPoolSubsystem();
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	void AddJob(const TFunction<void()>& Job);
	void AddJob(const TFunction<bool(double)>& Job);
	void AddJob(const TSharedRef<ISliceJob>& Job);

protected:
#if WITH_EDITOR
	void CreateThreadsForTest(const int32 NumberOfThreads);
#endif
	void CreateThreads();
	void DestroyThreads();
	TSharedRef<FPoolWorker, ESPMode::NotThreadSafe> GetAvailableThread() const;

private:
	TMap<TSharedRef<FPoolWorker, ESPMode::NotThreadSafe>, TSharedRef<FRunnableThread, ESPMode::NotThreadSafe>> ThreadPool;
	int32 ThreadCount;
};
