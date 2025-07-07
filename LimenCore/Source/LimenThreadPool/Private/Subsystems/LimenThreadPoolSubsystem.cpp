// Copyright © 2024 FaceSoftware. All rights reserved.


#include "Subsystems/LimenThreadPoolSubsystem.h"

#include "Async/Async.h"
#include "BlueprintLibraries/LimenCoreStatics.h"
#include "Developer/LimenThreadPoolDeveloperSettings.h"
#include "HAL/Event.h"
#include "HAL/RunnableThread.h"
#include "LimenCore/Public/LogMacros/LimenLogMacros.h"


ULimenThreadPoolSubsystem::FPoolWorker::FPoolWorker() : QueueCondition(FSharedEventRef(EEventMode::ManualReset))
{
	bShouldStop = false;
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
		while (QueuedJobs.IsEmpty() && !bShouldStop)
		{
			// Wait for the event to be signaled
			QueueCondition->Wait();
		}

		TFunction<void()> CurrentJob = nullptr;
		if (QueuedJobs.Dequeue(CurrentJob))
		{
			QueuedJobsCount.store(QueuedJobsCount.load() - 1);
			check(CurrentJob != nullptr);
			CurrentJob();

			AsyncTask(ENamedThreads::GameThread, [this]
			{
				UE_LOG(LogLimen, Log, TEXT("ULimenThreadPoolSubsystem::FPoolWorker::Run: Thread %s completed job. In queue: %d"), *ThreadName, QueuedJobsCount.load());
			});	
		}
	}

	return 0;
}

void ULimenThreadPoolSubsystem::FPoolWorker::Stop()
{
	bShouldStop = true;
	QueueCondition->Trigger();
}

void ULimenThreadPoolSubsystem::FPoolWorker::QueueJob(const TFunction<void()>& Function)
{
	Function.CheckCallable();
	QueuedJobs.Enqueue(Function);
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

ULimenThreadPoolSubsystem::ULimenThreadPoolSubsystem()
{
	
}

bool ULimenThreadPoolSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	if (!Super::ShouldCreateSubsystem(Outer))
	{
		return false;
	}
	
	const ULimenThreadPoolDeveloperSettings* Settings = GetDefault<ULimenThreadPoolDeveloperSettings>();
	if (!Settings->bUseSubsystem)
	{
		return false;
	}

	return Settings->GetThreadCount() > 0;
}

void ULimenThreadPoolSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	CreateThreads();
}

void ULimenThreadPoolSubsystem::Deinitialize()
{
	DestroyThreads();
	
	Super::Deinitialize();
}

void ULimenThreadPoolSubsystem::AddJob(const TFunction<void()>& Function) const
{
	TSharedRef<FPoolWorker, ESPMode::NotThreadSafe> AvailableThread = GetAvailableThread();
	AvailableThread->QueueJob(Function);
	
	UE_LOG(LogLimen, Log, TEXT("ULimenThreadPoolSubsystem::FPoolWorker::Run New job queued to thread %s. In queue: %d"), *AvailableThread->ThreadName, AvailableThread->GetQueuedJobsCount());
}

void ULimenThreadPoolSubsystem::CreateThreads()
{
	const ULimenThreadPoolDeveloperSettings* Settings = GetDefault<ULimenThreadPoolDeveloperSettings>();
	for (int i = 0; i < Settings->GetThreadCount(); ++i)
	{
		const FString ThreadName = FString::Printf(TEXT("PoolThread_%d"), i);
		TSharedRef<FPoolWorker, ESPMode::NotThreadSafe> Worker = MakeShared<FPoolWorker, ESPMode::NotThreadSafe>();
		Worker->ThreadName = ThreadName;
		
		FRunnableThread* WorkerThreadRawPtr = FRunnableThread::Create(&Worker.Get(), *ThreadName, Settings->MemoryPerThreadInBytes, static_cast<EThreadPriority>(Settings->ThreadsPriority));
		TSharedRef<FRunnableThread, ESPMode::NotThreadSafe> WorkerThread = MakeShareable(WorkerThreadRawPtr);

		ThreadPool.Add(Worker, WorkerThread);
	}

	LIMEN_LOG(LogLimen, Log, this, "%d threads created successfully.", ThreadPool.Num());
}

void ULimenThreadPoolSubsystem::DestroyThreads()
{
	TArray<TSharedRef<FPoolWorker, ESPMode::NotThreadSafe>> ThreadArray;
	ThreadPool.GenerateKeyArray(ThreadArray);
	for (TSharedRef<FPoolWorker, ESPMode::NotThreadSafe>& ThreadWorker : ThreadArray)
	{
		ThreadWorker->DiscardWaitEvent();
		ThreadPool[ThreadWorker]->Kill(true);
	}
	ThreadPool.Empty();

	LIMEN_LOG(LogLimen, Log, this, "Threads destroyed.");
}

TSharedRef<ULimenThreadPoolSubsystem::FPoolWorker, ESPMode::NotThreadSafe> ULimenThreadPoolSubsystem::GetAvailableThread() const
{
	TArray<TSharedRef<FPoolWorker, ESPMode::NotThreadSafe>> ThreadWorkerArray;
	ThreadPool.GenerateKeyArray(ThreadWorkerArray);

	int32 SmallestJobsCount = TNumericLimits<int32>::Max();
	TSharedPtr<FPoolWorker, ESPMode::NotThreadSafe> OutThread;
	
	for (TSharedRef<FPoolWorker, ESPMode::NotThreadSafe>& ThreadWorker : ThreadWorkerArray)
	{
		if (ThreadWorker->GetQueuedJobsCount() < SmallestJobsCount)
		{
			SmallestJobsCount = ThreadWorker->GetQueuedJobsCount();
			OutThread = ThreadWorker.ToSharedPtr();
		}
	}
	
	return OutThread.ToSharedRef();
}
