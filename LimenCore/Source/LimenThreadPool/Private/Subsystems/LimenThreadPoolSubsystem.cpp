// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/LimenThreadPoolSubsystem.h"

#include "Developer/LimenThreadPoolDeveloperSettings.h"
#include "LimenCore/Public/LogMacros/LimenLogMacros.h"


ULimenThreadPoolSubsystem::FPoolWorker::FPoolWorker()
{
	bShouldStop = false;
	QueueCondition = MakeShareable(FPlatformProcess::GetSynchEventFromPool(true));
	check(QueueCondition.IsValid());
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
	while (true)
	{
		// Wait until there are jobs in the queue
		while (QueuedJobs.IsEmpty() && !bShouldStop)
		{
			// Wait for the event to be signaled
			QueueCondition->Wait();
		}

		if (bShouldStop)
		{
			break;
		}

		TFunction<void()> CurrentJob = nullptr;
		
		{
			// Lock the critical section before accessing the queue
			FScopeLock Lock(&QueueSection);
			// Dequeue and execute the job
			CurrentJob = QueuedJobs.Pop(true);
		}
		
		check(CurrentJob != nullptr);
		CurrentJob(); // Execute the job
		AsyncTask(ENamedThreads::GameThread, [this]
		{
			FScopeLock Lock(&QueueSection);
			UE_LOG(LogLimen, Log, TEXT("ULimenThreadPoolSubsystem::FPoolWorker::Run Thread %s completed job. In queue: %d"), *ThreadName, QueuedJobs.Num());
		});
	}

	return 0;
}

void ULimenThreadPoolSubsystem::FPoolWorker::Stop()
{
	bShouldStop = true;
	QueuedJobs.Empty();
	QueueCondition->Trigger();
}

void ULimenThreadPoolSubsystem::FPoolWorker::QueueJob(const TFunction<void()>& Function)
{
	check(Function);
	FScopeLock Lock(&QueueSection);
	Function.CheckCallable();
	QueuedJobs.Push(Function);
	QueueCondition->Trigger();
}

int32 ULimenThreadPoolSubsystem::FPoolWorker::GetQueuedJobsCount()
{
	FScopeLock Lock(&QueueSection);
	return QueuedJobs.Num();
}

void ULimenThreadPoolSubsystem::FPoolWorker::DiscardWaitEvent()
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
	return Settings->GetThreadCount() > 0;
}

void ULimenThreadPoolSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	CreateThreads();
}

void ULimenThreadPoolSubsystem::Deinitialize()
{
	Super::Deinitialize();

	DestroyThreads();
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

	LIMEN_LOG(LogLimen, Log, this, "Threads destroyed.", ThreadPool.Num());
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
