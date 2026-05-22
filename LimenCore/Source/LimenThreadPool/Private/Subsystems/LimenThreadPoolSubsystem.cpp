// Copyright © 2024 FaceSoftware. All rights reserved.


#include "Subsystems/LimenThreadPoolSubsystem.h"

#include "Async/Async.h"
#include "BlueprintLibraries/LimenCoreStatics.h"
#include "Developer/LimenThreadPoolDeveloperSettings.h"
#include "HAL/Event.h"
#include "HAL/RunnableThread.h"
#include "LimenCore/Public/LogMacros/LimenLogMacros.h"


ULimenThreadPoolSubsystem::FPoolWorker::FPoolWorker() : QueueCondition(FSharedEventRef(EEventMode::AutoReset))
{
	bShouldStop = false;
	QueuedJobsCount.store(0);
}

ULimenThreadPoolSubsystem::FPoolWorker::~FPoolWorker()
{
}

void ULimenThreadPoolSubsystem::FPoolWorker::Exit()
{
	
}

bool ULimenThreadPoolSubsystem::FPoolWorker::Init()
{
	return true;
}

uint32 ULimenThreadPoolSubsystem::FPoolWorker::Run()
{
	while (!bShouldStop.load())
	{
		// Wait until there are jobs in the queue
		TSharedPtr<ISliceJob> CurrentJob = nullptr;
		if (!QueuedJobs.Dequeue(CurrentJob))
		{
			if (bShouldStop.load()) break;
			QueueCondition->Wait();
			continue;
		}

		QueuedJobsCount.fetch_sub(1);
		check(CurrentJob.IsValid());


		if (!CurrentJob->ExecuteSlice(SliceBudget))
		{
			QueueJob(CurrentJob.ToSharedRef());
			continue;
		}

#if !UE_BUILD_SHIPPING && !UE_BUILD_TEST
		AsyncTask(ENamedThreads::GameThread, [ThreadNameCopy = FString(ThreadName), QueuedJobsCopy = QueuedJobsCount.load()]
		{
			UE_LOG(LogLimen, Log, TEXT("ULimenThreadPoolSubsystem::FPoolWorker::Run: Thread %s completed job. In queue: %d"), *ThreadNameCopy, QueuedJobsCopy);
		});		
#endif
	}

	return 0;
}

void ULimenThreadPoolSubsystem::FPoolWorker::Stop()
{
	bShouldStop = true;
	QueueCondition->Trigger();
}

void ULimenThreadPoolSubsystem::FPoolWorker::QueueJob(const TSharedRef<ISliceJob>& NewJob)
{
	QueuedJobs.Enqueue(NewJob);
	++QueuedJobsCount;
	QueueCondition->Trigger();
}

int32 ULimenThreadPoolSubsystem::FPoolWorker::GetQueuedJobsCount() const
{
	return QueuedJobsCount.load();
}

void ULimenThreadPoolSubsystem::FPoolWorker::DiscardWaitEvent() const
{
	QueueCondition->Trigger();
}

ULimenThreadPoolSubsystem::ULimenThreadPoolSubsystem(): ThreadCount(0)
{
}

bool ULimenThreadPoolSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	if (!Super::ShouldCreateSubsystem(Outer))
	{
		return false;
	}

	if (FPlatformMisc::NumberOfCoresIncludingHyperthreads() <= 1)
	{
		LIMEN_LOG(LogLimen, Warning, this, TEXT("Skipping thread pool subsystem: only one core available."))
		return false;
	}

	const ULimenThreadPoolDeveloperSettings* Settings = GetDefault<ULimenThreadPoolDeveloperSettings>();
	if (!Settings || !Settings->bUseSubsystem)
	{
		LIMEN_LOG(LogLimen, Warning, this, TEXT("Thread pool subsystem disabled via developer settings."))
		return false;
	}

	if (Settings->GetThreadCount() <= 0)
	{
		LIMEN_LOG(LogLimen, Warning, this, TEXT("Thread pool subsystem skipped: settings thread count <= 0."))
		return false;
	}

	return true;
}

void ULimenThreadPoolSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	const ULimenThreadPoolDeveloperSettings* Settings = GetDefault<ULimenThreadPoolDeveloperSettings>();
	ThreadCount = Settings->GetThreadCount();


	const int32 MaxThreads = FPlatformMisc::NumberOfCoresIncludingHyperthreads() - 1;
	if (const int32 DesiredThreads = Settings->GetThreadCount(); MaxThreads < DesiredThreads)
	{
		LIMEN_LOG(LogLimen, Warning, this, TEXT("Requested thread count (%d) exceeds available cores (%d). Clamping to %d."), DesiredThreads, MaxThreads, MaxThreads);
		ThreadCount = MaxThreads > 0 ? MaxThreads : 1;
	}

	CreateThreads();
}

void ULimenThreadPoolSubsystem::Deinitialize()
{
	DestroyThreads();
	
	Super::Deinitialize();
}

void ULimenThreadPoolSubsystem::AddJob(const TFunction<void()>& Job)
{
	AddJob(MakeShared<FUnslicedJob>(Job));
}

void ULimenThreadPoolSubsystem::AddJob(const TFunction<bool(double)>& Job)
{
	AddJob(MakeShared<FDummySliceJob>(Job));
}

void ULimenThreadPoolSubsystem::AddJob(const TSharedRef<ISliceJob>& Job)
{
	// Ironically, not thread safe.
	// But it doesn't make sense to queue work into a thread if already outside the game thread.
	check(IsInGameThread());

	TSharedRef<FPoolWorker, ESPMode::NotThreadSafe> AvailableThread = GetAvailableThread();
	AvailableThread->QueueJob(Job);
	
	LIMEN_LOG(LogLimen, Log, this, TEXT("ULimenThreadPoolSubsystem::FPoolWorker::Run New job queued to thread %s. In queue: %d"), *AvailableThread->ThreadName, AvailableThread->GetQueuedJobsCount());
}

void ULimenThreadPoolSubsystem::CreateThreads()
{
	const ULimenThreadPoolDeveloperSettings* Settings = GetDefault<ULimenThreadPoolDeveloperSettings>();
	for (int i = 0; i < ThreadCount; ++i)
	{
		const FString ThreadName = FString::Printf(TEXT("PoolThread_%d"), i);
		TSharedRef<FPoolWorker, ESPMode::NotThreadSafe> Worker = MakeShared<FPoolWorker, ESPMode::NotThreadSafe>();
		Worker->ThreadName = ThreadName;
		
		FRunnableThread* WorkerThreadRawPtr = FRunnableThread::Create(&Worker.Get(), *ThreadName, Settings->MemoryPerThreadInBytes, static_cast<EThreadPriority>(Settings->ThreadsPriority));
		TSharedRef<FRunnableThread, ESPMode::NotThreadSafe> WorkerThread = MakeShareable(WorkerThreadRawPtr);

		ThreadPool.Add(Worker, WorkerThread);
	}

	LIMEN_LOG(LogLimen, Log, this, TEXT("%d threads created successfully."), ThreadPool.Num());
}

void ULimenThreadPoolSubsystem::DestroyThreads()
{
	TArray<TSharedRef<FPoolWorker, ESPMode::NotThreadSafe>> Workers;
	ThreadPool.GenerateKeyArray(Workers);
	for (const auto& W : Workers) { W->Stop(); }
	for (auto& W : Workers) { ThreadPool[W]->WaitForCompletion(); }
	ThreadPool.Empty();

	LIMEN_LOG(LogLimen, Log, this, TEXT("Threads destroyed."));
}

TSharedRef<ULimenThreadPoolSubsystem::FPoolWorker, ESPMode::NotThreadSafe> ULimenThreadPoolSubsystem::GetAvailableThread() const
{
	check(IsInGameThread());
	check(ThreadPool.Num() > 0);

	TSharedRef<FPoolWorker, ESPMode::NotThreadSafe> Best = ThreadPool.CreateConstIterator().Key();
	int32 BestCount = Best->GetQueuedJobsCount();

	for (const auto& Pair : ThreadPool)
	{
		if (const int32 WorkCount = Pair.Key->GetQueuedJobsCount(); WorkCount < BestCount)
		{
			Best = Pair.Key;
			BestCount = WorkCount;
		}
	}

	return Best;
}

void ULimenThreadPoolSubsystem::CreateThreadsForTest(const int32 NumberOfThreads)
{
	const ULimenThreadPoolDeveloperSettings* Settings = GetDefault<ULimenThreadPoolDeveloperSettings>();
	for (int i = 0; i < NumberOfThreads; ++i)
	{
		const FString ThreadName = FString::Printf(TEXT("PoolThread_%d"), i);
		TSharedRef<FPoolWorker, ESPMode::NotThreadSafe> Worker = MakeShared<FPoolWorker, ESPMode::NotThreadSafe>();
		Worker->ThreadName = ThreadName;
		
		FRunnableThread* WorkerThreadRawPtr = FRunnableThread::Create(&Worker.Get(), *ThreadName, Settings->MemoryPerThreadInBytes, static_cast<EThreadPriority>(Settings->ThreadsPriority));
		TSharedRef<FRunnableThread, ESPMode::NotThreadSafe> WorkerThread = MakeShareable(WorkerThreadRawPtr);

		ThreadPool.Add(Worker, WorkerThread);
	}

	LIMEN_LOG(LogLimen, Log, this, TEXT("%d threads created successfully."), ThreadPool.Num());
}
