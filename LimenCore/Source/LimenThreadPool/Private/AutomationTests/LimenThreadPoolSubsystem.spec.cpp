#include "LogMacros/LimenLogMacros.h"
#include "Misc/AutomationTest.h"
#include "Subsystems/LimenThreadPoolSubsystem.h"


BEGIN_DEFINE_SPEC(FThreadPoolSpec, "LimenFramework.LimenCore.Source.LimenThreadPool.ThreadPoolTest",
				  EAutomationTestFlags::EditorContext | EAutomationTestFlags::CommandletContext |
				  EAutomationTestFlags::HighPriority | EAutomationTestFlags::ProductFilter)


class FPrimeCheckTest final : public ULimenThreadPoolSubsystem::ISliceJob
{
public:
	FSimpleDelegate OnThreadDone;

	explicit FPrimeCheckTest(uint64 InNumber)
		: N(InNumber)
	{
		// Trivial cases handled on first ExecuteSlice
	}

	virtual bool ExecuteSlice(const double BudgetSeconds) override
	{
		if (bFinished)
		{
			OnThreadDone.Execute();
			return true;
		}

		const FBudgetHelper Helper(BudgetSeconds);

		// First-time initialization and trivial checks
		if (!bInitialized)
		{
			if (N < 2)
			{
				OnThreadDone.Execute();
				bIsPrime = false; bFinished = true; return true;
			}
			if ((N & 1ull) == 0ull)
			{
				OnThreadDone.Execute();
				bIsPrime = (N == 2ull); bFinished = true; return true;
			}
			NextDivisor = 3;                 // start at first odd divisor
			bInitialized = true;
		}

		// Test odd divisors; use N/NextDivisor to avoid i*i overflow
		for (; (uint64)NextDivisor <= N / (uint64)NextDivisor; NextDivisor += 2)
		{
			if ((N % (uint64)NextDivisor) == 0ull)
			{
				OnThreadDone.Execute();
				bIsPrime = false; bFinished = true; return true;
			}

			if (Helper.ShouldYield())
			{
				Slices++;
				return false; // yield — state (NextDivisor) is preserved
			}
		}

		// If we got here, no divisors found
		bIsPrime = true;
		bFinished = true;
		OnThreadDone.Execute();
		return true;
	}

	bool IsFinished() const { return bFinished; }
	bool GetResult() const  { return bIsPrime; }
	uint64 GetNumber() const { return N; }
	int32 GetProgressDivisor() const { return NextDivisor; }
	int32 GetSlicesExecuted() const { return Slices; }

private:
	uint64 N = 0;
	int32  NextDivisor = 3;   // resumes here on next slice
	bool   bInitialized = false;
	bool   bFinished    = false;
	bool   bIsPrime     = false;
	int32 Slices = 0;
};

FORCEINLINE static bool IsPrime64(const uint64 N)
{
	if (N < 2) return false;
	if ((N & 1ull) == 0ull) return N == 2;
	if (N % 3ull == 0ull)   return N == 3;

	// check 6k ± 1
	for (uint64 i = 5; i <= N / i; i += 6)
	{
		if (N % i == 0ull || N % (i + 2) == 0ull) return false;
	}
	return true;
}

FORCEINLINE static uint64 NextPrime64(uint64 n)
{
	if (n <= 2) return 2;
	if ((n & 1ull) == 0ull) ++n;            // make odd
	while (!IsPrime64(n)) n += 2;           // skip evens
	return n;
}

std::atomic<int32> CompletedJobs;
int32 NumberOfJobs;
ULimenThreadPoolSubsystem* ThreadPoolSubsystem = nullptr;
TArray<TSharedPtr<FPrimeCheckTest>> Jobs;

END_DEFINE_SPEC(FThreadPoolSpec)


void FThreadPoolSpec::Define()
{
	Describe(TEXT("When work is queued into the thread pool"), [this]
	{
		BeforeEach([this]
		{
			CompletedJobs = 0;
#if PLATFORM_WINDOWS
			NumberOfJobs = FWindowsPlatformMisc::NumberOfCores();
#else
			NumberOfJobs = FGenericPlatformMisc::NumberOfCores();
#endif
			NumberOfJobs *= 5;
			ThreadPoolSubsystem = NewObject<ULimenThreadPoolSubsystem>(GetTransientOuterForRename(ULimenThreadPoolSubsystem::StaticClass()));
			ThreadPoolSubsystem->CreateThreadsForTest(FGenericPlatformMisc::NumberOfCores());
		});
		
		LatentIt(TEXT("should confirm the work is completed successfully"), [this] (const FDoneDelegate& DoneDelegate)
		{
			TArray<uint64> PrimeList;
			PrimeList.Push(1000003);
			PrimeList.Push(1000033);
			PrimeList.Push(1000037);
			PrimeList.Push(1000039);

			auto OnJobDone = [this, DoneDelegate]
			{
				if (++CompletedJobs != NumberOfJobs) return;

				// ensure we run on GT (if not already)
				AsyncTask(ENamedThreads::GameThread, [this, DoneDelegate]
				{
					for (const auto& Job : Jobs)
					DoneDelegate.ExecuteIfBound();
				});
			};

			int32 PrimeIndex = 0;
			for (int32 i = 0; i < NumberOfJobs; ++i)
			{
				if (!PrimeList.IsValidIndex(PrimeIndex)) {PrimeIndex = 0; }

				TSharedRef<FPrimeCheckTest> Temp = MakeShared<FPrimeCheckTest>(PrimeList[PrimeIndex]);
				Temp->OnThreadDone.BindLambda(OnJobDone);
				Jobs.Push(Temp);
			}

			for (const TSharedPtr<FPrimeCheckTest>& Job : Jobs)
			{
				ThreadPoolSubsystem->AddJob(StaticCastSharedRef<ULimenThreadPoolSubsystem::ISliceJob>(Job.ToSharedRef()));
			}
		});

		AfterEach([this]
		{
			int32 JobNumber = 1;
			for (const auto& Job : Jobs)
			{
				STATIC_LIMEN_LOG(LogLimenCore, Log, FString(TEXT("FThreadPoolSpec")), TEXT("Job %d executed %d slices!"), JobNumber, Job->GetSlicesExecuted());
				JobNumber++;
			}
			Jobs.Empty();
			
			if (ThreadPoolSubsystem)
			{
				ThreadPoolSubsystem->DestroyThreads();
				ThreadPoolSubsystem->RemoveFromRoot();
				ThreadPoolSubsystem->MarkAsGarbage();
				ThreadPoolSubsystem = nullptr;
			}
		});
	});
}
