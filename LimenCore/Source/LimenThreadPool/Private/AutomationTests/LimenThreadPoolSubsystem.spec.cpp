#include "Misc/AutomationTest.h"
#include "Subsystems/LimenThreadPoolSubsystem.h"


BEGIN_DEFINE_SPEC(FThreadPoolSpec, "LimenFramework.LimenCore.Source.LimenThreadPool.ThreadPoolTest",
				  EAutomationTestFlags::EditorContext | EAutomationTestFlags::CommandletContext |
				  EAutomationTestFlags::HighPriority | EAutomationTestFlags::ProductFilter)

std::atomic<int32> CompletedJobs;
int32 NumberOfJobs;
ULimenThreadPoolSubsystem* ThreadPoolSubsystem = nullptr;


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

		FBudgetHelper Helper(BudgetSeconds);

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

private:
	uint64 N = 0;
	int32  NextDivisor = 3;   // resumes here on next slice
	bool   bInitialized = false;
	bool   bFinished    = false;
	bool   bIsPrime     = false;
};

FORCEINLINE bool IsPrime64(uint64 n)
{
	if (n < 2) return false;
	if ((n & 1ull) == 0ull) return n == 2;
	if (n % 3ull == 0ull)   return n == 3;

	// check 6k ± 1
	for (uint64 i = 5; i <= n / i; i += 6)
	{
		if (n % i == 0ull || n % (i + 2) == 0ull) return false;
	}
	return true;
}

FORCEINLINE uint64 NextPrime64(uint64 n)
{
	if (n <= 2) return 2;
	if ((n & 1ull) == 0ull) ++n;            // make odd
	while (!IsPrime64(n)) n += 2;           // skip evens
	return n;
}

END_DEFINE_SPEC(FThreadPoolSpec)


void FThreadPoolSpec::Define()
{
	Describe(TEXT("When work is queued into the thread pool"), [this]
	{
		BeforeEach([this]
		{
			CompletedJobs = 0;
			NumberOfJobs = FGenericPlatformMisc::NumberOfCores() * 1;
			ThreadPoolSubsystem = NewObject<ULimenThreadPoolSubsystem>(GetTransientOuterForRename(ULimenThreadPoolSubsystem::StaticClass()));
			ThreadPoolSubsystem->CreateThreadsForTest(FGenericPlatformMisc::NumberOfCores());
		});
		
		LatentIt(TEXT("should confirm the work is completed successfully"), [this] (const FDoneDelegate& DoneDelegate)
		{
			auto OnJobDone = [this, DoneDelegate, Total = NumberOfJobs]
			{
				// ensure we run on GT (if not already)
				AsyncTask(ENamedThreads::GameThread, [this, DoneDelegate, Total]
				{
					if (++CompletedJobs == Total) DoneDelegate.ExecuteIfBound();
				});
			};

			TArray<uint64> PrimeList;
			PrimeList.Push(1152921504606847009ull);
			PrimeList.Push(1152921504606847067ull);
			PrimeList.Push(1152921504606847081ull);
			PrimeList.Push(1152921504606847123ull);
			PrimeList.Push(1152921504606847127ull);
			PrimeList.Push(1152921504606847189ull);
			PrimeList.Push(1152921504606847201ull);
			PrimeList.Push(1152921504606847229ull);
			PrimeList.Push(1152921504606847253ull);
			PrimeList.Push(1152921504606847291ull);
			PrimeList.Push(1152921504606847301ull);
			PrimeList.Push(1152921504606847309ull);
			PrimeList.Push(1152921504606847363ull);
			PrimeList.Push(1152921504606847411ull);
			PrimeList.Push(1152921504606847483ull);
			PrimeList.Push(1152921504606847517ull);

			TArray<TSharedPtr<FPrimeCheckTest>> Jobs;
			int32 PrimeIndex = 0;
			for (int32 i = 0; i < NumberOfJobs; ++i)
			{
				if (!PrimeList.IsValidIndex(PrimeIndex)) {PrimeIndex = 0; }

				TSharedRef<FPrimeCheckTest> Temp = MakeShared<FPrimeCheckTest>(PrimeList[PrimeIndex]);
				Jobs.Push(Temp);
				Temp->OnThreadDone.BindLambda(OnJobDone);
			}

			for (const TSharedPtr<FPrimeCheckTest>& Job : Jobs)
			{
				ThreadPoolSubsystem->AddJob(StaticCastSharedRef<ULimenThreadPoolSubsystem::ISliceJob>(Job.ToSharedRef()));
			}
		});

		AfterEach([this]
		{
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
