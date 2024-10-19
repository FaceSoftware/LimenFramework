// Copyright Face Software. All Rights Reserved.


#include "Actors/LimenProceduralMapBuilder.h"

#include "Subsystems/LimenGlobalRandomStreamSubsystem.h"
#include "BlueprintLibraries/LimenCoreStatics.h"
#include "BlueprintLibraries/LimenMath.h"
#include "DataAssets/ProceduralMapDataAsset.h"
#include "Managers/LimenProceduralMapManager.h"
#include "Namespaces/GlobalInfo.h"


ALimenProceduralMapBuilder::ALimenProceduralMapBuilder(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
	SetHidden(true);
	Seed = 1;
	bIsMapBuilt = false;
	NumMapsBuilt = 0;
	CurrentMapIndex = -1;
	bShouldStopMapBuilderThread = false;
}

void ALimenProceduralMapBuilder::BeginPlay()
{
	Super::BeginPlay();

	OnBuildProcessStarted.AddUniqueDynamic(this, &ThisClass::OnBuildingMapStarted);
	OnBuildProcessFinished.AddUniqueDynamic(this, &ThisClass::OnBuildingMapFinished);
}

void ALimenProceduralMapBuilder::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (MapInitializerRunnable.IsValid())
	{
		if (!MapInitializerRunnable->HasGameThreadStageCompleteResponse())
		{
			TPromise<bool>& Success = GetGameThreadStageCompletePromise();
			Success.SetValue(false);
		}
	}

	bShouldStopMapBuilderThread = true;
	if (MapInitializerThread.IsValid())
	{
		// Suspend first or the thread will keep running and crash eventually because it's accessing stale pointers.
		MapInitializerRunnable->RequestEarlyStop();
		MapInitializerThread->Suspend(false);
		MapInitializerThread->Kill(true);
	}
	
	Super::EndPlay(EndPlayReason);
}

void ALimenProceduralMapBuilder::StartBuildProcess()
{
	PreBuildStart();
	
	if (MapInitializerThread.IsValid())
	{
		MapInitializerThread->Suspend(true);
		MapInitializerThread->Kill(false);
		MapInitializerThread.Reset();
	}
	
	MapInitializerRunnable.Reset();
	MapInitializerRunnable = MakeUnique<FMapBuilderThread>(this, CalculationSteps, GameThreadSteps, CleanupSteps);
	MapInitializerThread = TUniquePtr<FRunnableThread>(FRunnableThread::Create(MapInitializerRunnable.Get(), TEXT("MapBuilderThread"), 10024, TPri_AboveNormal));
}

void ALimenProceduralMapBuilder::SetSeed(const uint64 NewSeed)
{
	Seed = NewSeed;
}

uint64 ALimenProceduralMapBuilder::GetSeed() const
{
	return Seed;
}

void ALimenProceduralMapBuilder::UnloadMap()
{
	if (CurrentMapManager != nullptr)
	{
		CurrentMapManager->Destroy();
		CurrentMapManager = nullptr;
	}

	OnMapUnloaded.Broadcast(CurrentMapIndex);
}

void ALimenProceduralMapBuilder::DestroyPreviousMap()
{
	
}

bool ALimenProceduralMapBuilder::SetCurrentMapDataAssetByName(const FName MapName)
{
	const auto Map = Maps.Find(MapName);
	if (!IsValid(*Map))
	{
		ULimenCoreStatics::LimenLog(this, FString::Printf(TEXT("Unable to find a map with name: %s"), *MapName.ToString()), ELogType::Error);
		return false;
	}
	
	SetCurrentMapDataAssetByRef(*Map);
	return true;
}

bool ALimenProceduralMapBuilder::SetCurrentMapDataAssetByRef(UProceduralMapDataAsset* MapDataAsset)
{
	if (!IsValid(MapDataAsset))
	{
		ULimenCoreStatics::LimenLog(this, TEXT("Invalid map data"), ELogType::Error);
		return false;
	}
	
	CurrentMap = MapDataAsset;
	TArray<UProceduralMapDataAsset*> MapsData;
	Maps.GenerateValueArray(MapsData);

	bool Found = false;
	for (int i = 0; i < MapsData.Num(); ++i)
	{
		if (MapsData[i] == MapDataAsset)
		{
			CurrentMapIndex = i;
			Found = true;
			break;
		}
	}

	if (!Found)
	{
		return false;
	}
	
	OnMapLoaded.Broadcast(CurrentMapIndex);
	return true;
}

bool ALimenProceduralMapBuilder::LoadNextMap()
{
	TArray<UProceduralMapDataAsset*> MapsData;
	Maps.GenerateValueArray(MapsData);
	if (!MapsData.IsValidIndex(++CurrentMapIndex))
	{
		return false;
	}

	SetCurrentMapDataAssetByRef(MapsData[CurrentMapIndex]);
	return true;
}

bool ALimenProceduralMapBuilder::LoadMapWithIndex(const int Index)
{
	TArray<UProceduralMapDataAsset*> MapsData;
	Maps.GenerateValueArray(MapsData);
	if (!MapsData.IsValidIndex(Index))
	{
		return false;
	}
	
	CurrentMapIndex = Index;
	SetCurrentMapDataAssetByRef(MapsData[CurrentMapIndex]);
	return true;
}

int ALimenProceduralMapBuilder::GetCurrentMapIndex() const
{
	return CurrentMapIndex;
}

UProceduralMapDataAsset* ALimenProceduralMapBuilder::GetCurrentMapData() const
{
	return CurrentMap.Get();
}

ALimenProceduralMapManager* ALimenProceduralMapBuilder::GetCurrentMapManager() const
{
	return CurrentMapManager.Get();
}

bool ALimenProceduralMapBuilder::IsWorking_Implementation() const
{
	return bIsWorking && NumMapsBuilt == 0;
}

TSharedRef<FRandomStream, ESPMode::ThreadSafe> ALimenProceduralMapBuilder::GetRandomStream()
{
	return ULimenGlobalRandomStreamSubsystem::Get()->GetGlobalRandomStream();
}

void ALimenProceduralMapBuilder::BP_AddCalculationStep(const FName FunctionName)
{
	FBuildStep New;
	New.BindUFunction(this, FunctionName);
	CalculationSteps.Push(New);
}

void ALimenProceduralMapBuilder::BP_AddGameThreadStep(const FName FunctionName)
{
	FBuildStep New;
	New.BindUFunction(this, FunctionName);
	GameThreadSteps.Push(New);
}

void ALimenProceduralMapBuilder::BP_AddCleanupStep(const FName FunctionName)
{
	FBuildStep New;
	New.BindUFunction(this, FunctionName);
	CleanupSteps.Push(New);
}

void ALimenProceduralMapBuilder::OnBuildingMapStarted()
{
	ULimenCoreStatics::LimenLog(this, TEXT("Building Map..."));
	bIsWorking = true;
}

void ALimenProceduralMapBuilder::OnBuildingMapFinished()
{	
	ULimenCoreStatics::LimenLog(this, TEXT("Map Built"));
	bIsWorking = false;
	NumMapsBuilt++;

	check(CurrentMap != nullptr);
	if (CurrentMapManager != nullptr)
	{
		CurrentMapManager->MapBuilt(CurrentMap.Get());
	}

	SetupMapCompletionListener();
}

TPromise<bool>& ALimenProceduralMapBuilder::GetGameThreadStageCompletePromise()
{
	check(MapInitializerRunnable.IsValid())
	return MapInitializerRunnable->GetGameThreadStageCompletePromise();
}

bool ALimenProceduralMapBuilder::ShouldStopMapBuilderThread() const
{
	return bShouldStopMapBuilderThread;
}

void ALimenProceduralMapBuilder::FinalLevelCompleted_Implementation()
{
}

void ALimenProceduralMapBuilder::SetupMapCompletionListener()
{
	if (CurrentMapManager == nullptr)
	{
		return;
	}
	
	CurrentMapManager->OnMapComplete.AddLambda([this]
	{
		if (LoadNextMap())
		{
			StartBuildProcess();
		}
		else
		{
			FinalLevelCompleted();
		}
	});
}

FMapBuilderThread::FMapBuilderThread(ALimenProceduralMapBuilder* InInstance,
									 const TArray<FBuildStep>& InCalculationSteps, const TArray<FBuildStep>& InGameThreadSteps,
									 const TArray<FBuildStep>& InCleanupSteps)
{
	check(InInstance)
	InstanceWeakPtr = TWeakObjectPtr<ALimenProceduralMapBuilder>(InInstance);
	
	CalculationSteps = InCalculationSteps;
	GameThreadSteps = InGameThreadSteps;
	CleanupSteps = InCleanupSteps;

	GlobalSteps = CalculationSteps.Num() + GameThreadSteps.Num() + CleanupSteps.Num();
	GlobalProgress = 0.f;
	SingleStepProgress = 100.f / static_cast<float>(GlobalSteps);

	bShouldStop = false;
	bGotGameThreadStageCompleteResponse = false;
}

FMapBuilderThread::~FMapBuilderThread()
{
	InstanceWeakPtr.Reset();
	CalculationSteps.Empty();
	GameThreadSteps.Empty();
	CleanupSteps.Empty();
}

bool FMapBuilderThread::Init()
{
	return InstanceWeakPtr.IsValid();
}

uint32 FMapBuilderThread::Run()
{
	check(InstanceWeakPtr.IsValid())

	AsyncTask(ENamedThreads::GameThread, [this]
	{
		InstanceWeakPtr->OnBuildProcessStarted.Broadcast();

		// Otherwise will never be called with 0%
		InstanceWeakPtr->OnBuildProcessUpdated.Broadcast(0.f, EMapBuildStage::Calculations, 0, CalculationSteps.Num());
	});

	if (const uint32 ExitCode = Calculate(); ExitCode != LimenThreading::ExitCodes::Success)
	{
		check(ExitCode != LimenThreading::ExitCodes::CriticalError)
		return ExitCode;
	}

	if (const uint32 ExitCode = Spawn(); ExitCode != LimenThreading::ExitCodes::Success)
	{
		check(ExitCode != LimenThreading::ExitCodes::CriticalError)
		return ExitCode;
	}
	
	if (const uint32 ExitCode = Cleanup(); ExitCode != LimenThreading::ExitCodes::Success)
	{
		check(ExitCode != LimenThreading::ExitCodes::CriticalError)
		return ExitCode;
	}
	
	AsyncTask(ENamedThreads::GameThread, [this]
	{
		if (IsValid(InstanceWeakPtr->CurrentMap->GetMapManagerClass()))
		{
			InstanceWeakPtr->CurrentMapManager = InstanceWeakPtr->GetWorld()->SpawnActor<ALimenProceduralMapManager>(InstanceWeakPtr->CurrentMap->GetMapManagerClass());
		}
		
		InstanceWeakPtr->bIsMapBuilt = true;
		InstanceWeakPtr->OnBuildProcessFinished.Broadcast();
	});
	
	return LimenThreading::ExitCodes::Success;
}

void FMapBuilderThread::Stop()
{
	bShouldStop = true;
}

void FMapBuilderThread::Exit()
{
}

TPromise<bool>& FMapBuilderThread::GetGameThreadStageCompletePromise()
{
	return GameThreadStageComplete;
}

bool FMapBuilderThread::HasGameThreadStageCompleteResponse() const
{
	return bGotGameThreadStageCompleteResponse;
}

void FMapBuilderThread::RequestEarlyStop()
{
	bShouldStop = true;
}

uint32 FMapBuilderThread::Calculate()
{
	if (CalculationSteps.IsEmpty())
	{
		return LimenThreading::ExitCodes::Success;
	}
	
	for (int i = 0; i < CalculationSteps.Num(); ++i)
	{
		if (bShouldStop)
		{
			return LimenThreading::ExitCodes::EarlyStop;
		}
		
		check(CalculationSteps[i].IsBound())
		CalculationSteps[i].Execute();
		GlobalProgress += SingleStepProgress;
		
		auto Progress = ULimenMath::RoundToNearestDouble(GlobalProgress);
		AsyncTask(ENamedThreads::GameThread, [this, Progress, i]
		{
			if (bShouldStop)
			{
				return;
			}

			InstanceWeakPtr->OnBuildProcessUpdated.Broadcast(Progress, EMapBuildStage::Calculations, i + 1.f, CalculationSteps.Num());
		});
	}

	return LimenThreading::ExitCodes::Success;
}

uint32 FMapBuilderThread::Spawn()
{	
	if (GameThreadSteps.IsEmpty())
	{
		return LimenThreading::ExitCodes::Success;
	}
	
	for (int i = 0; i < GameThreadSteps.Num(); ++i)
	{
		if (bShouldStop)
		{
			return LimenThreading::ExitCodes::EarlyStop;
		}
		
		check(GameThreadSteps[i].IsBound())
		GlobalProgress += SingleStepProgress;

		auto Progress = ULimenMath::RoundToNearestDouble(GlobalProgress);
		AsyncTask(ENamedThreads::GameThread, [this, Progress, i]
		{
			if (bShouldStop)
			{
				return;
			}
			
			GameThreadSteps[i].Execute();
			InstanceWeakPtr->OnBuildProcessUpdated.Broadcast(Progress, EMapBuildStage::Spawning, i + 1.f, GameThreadSteps.Num());
		});
	}
	
	const TFuture<bool> Future = GameThreadStageComplete.GetFuture();
	Future.Wait();
	bGotGameThreadStageCompleteResponse = true;
	if (!Future.Get())
	{
		return LimenThreading::ExitCodes::NonCriticalError;
	}
	
	return LimenThreading::ExitCodes::Success;
}

uint32 FMapBuilderThread::Cleanup()
{
	if (CleanupSteps.IsEmpty())
	{
		return LimenThreading::ExitCodes::Success;
	}
	
	for (int i = 0; i < CleanupSteps.Num(); ++i)
	{
		if (bShouldStop)
		{
			return LimenThreading::ExitCodes::EarlyStop;
		}
		
		check(CleanupSteps[i].IsBound())
		GlobalProgress += SingleStepProgress;
		CleanupSteps[i].Execute();

		auto Progress = ULimenMath::RoundToNearestDouble(GlobalProgress);
		AsyncTask(ENamedThreads::GameThread, [this, Progress, i]
		{
			if (bShouldStop)
			{
				return;
			}

			InstanceWeakPtr->OnBuildProcessUpdated.Broadcast(Progress, EMapBuildStage::Cleanup, i + 1.f, CleanupSteps.Num());
		});
	}

	return LimenThreading::ExitCodes::Success;
}

