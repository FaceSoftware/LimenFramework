#include "Misc/AutomationTest.h"
#include "Subsystems/LimenThreadPoolSubsystem.h"

BEGIN_DEFINE_SPEC(FThreadPoolSpec, "LimenFramework.LimenCore.Source.LimenThreadPool.ThreadPoolTest",
				  EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::FeatureMask |
				  EAutomationTestFlags::HighPriority | EAutomationTestFlags::ProductFilter)

int32 CompletedJobs;
int32 NumberOfJobs;
ULimenThreadPoolSubsystem* ThreadPoolSubsystem = nullptr;

static bool IsPrime(const int64 Test);

END_DEFINE_SPEC(FThreadPoolSpec)


void FThreadPoolSpec::Define()
{
	Describe(TEXT("When work is queued into the thread pool"), [this]
	{
		BeforeEach([this]
		{
			CompletedJobs = 0;
			NumberOfJobs = FWindowsPlatformMisc::NumberOfCores() * 5;
			ThreadPoolSubsystem = NewObject<ULimenThreadPoolSubsystem>(GetTransientOuterForRename(ULimenThreadPoolSubsystem::StaticClass()));
			ThreadPoolSubsystem->CreateThreads();
		});
		
		LatentIt(TEXT("should confirm the work is completed successfully"), [this] (const FDoneDelegate DoneDelegate)
		{
			TArray<TFunction<void()>> Jobs;
			for (int i = 0; i < NumberOfJobs; ++i)
			{
				TFunction<void()> Temp = [this, DoneDelegate]
				{
					for (int64 i = 0; i < TNumericLimits<int64>::Max(); i++)
					{
						IsPrime(i);
					}
					AsyncTask(ENamedThreads::GameThread,[this, DoneDelegate]
					{
						CompletedJobs++;
						if (CompletedJobs == NumberOfJobs)
						{
							DoneDelegate.Execute();
						}
					});
				};
				Jobs.Push(Temp);
			}
			
			for (const TFunction<void()>& Job : Jobs)
			{
				 ThreadPoolSubsystem->AddJob(Job);
			}
		});

		AfterEach([this]
		{
			ThreadPoolSubsystem->DestroyThreads();
			ThreadPoolSubsystem->ConditionalBeginDestroy();
		});
	});
}

bool FThreadPoolSpec::IsPrime(const int64 Test)
{
	bool bIsPrime = true;

	// 0 and 1 are not prime numbers
	if (Test == 0 || Test == 1)
	{
		bIsPrime = false;
	}

	for (int i = 2; i <= Test / 2; ++i)
	{
		if (Test % i == 0)
		{
			bIsPrime = false;
			break;
		}
	}

	return bIsPrime;
}
