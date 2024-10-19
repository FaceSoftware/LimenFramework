// Copyright Face Software. All Rights Reserved.


#include "GameInstances/LimenGameInstance.h"

#include "BlueprintLibraries/LimenCoreStatics.h"
#include "Subsystems/LimenLevelManagerSubsystem.h"
#include "Subsystems/LimenLevelTransitionSubsystem.h"


TSharedPtr<FShaderCompiler> FShaderCompiler::ShaderCompiler = nullptr;

TSharedPtr<FShaderCompiler> FShaderCompiler::Get()
{
	if (!ShaderCompiler.IsValid())
	{
		ShaderCompiler = TSharedPtr<FShaderCompiler>(new FShaderCompiler);
	}
	
	return ShaderCompiler;
}

void FShaderCompiler::StartCompiling(UWorld* InWorld, const FShaderPipelineCache::BatchMode InBatchMode)
{
	World = InWorld;
	check(IsValid(World));
	BatchMode = InBatchMode;
	StartShaderBatching();
}

bool FShaderCompiler::IsCompiling() const
{
	return bIsCompiling;
}

void FShaderCompiler::StopCompiling()
{
	bStopCompiling = true;
}

void FShaderCompiler::StartShaderBatching()
{
#if !UE_BUILD_SHIPPING
	const FString CompilationMessage = FString::Printf(TEXT("Compiling %lld shaders"), TotalShaders);
	ULimenCoreStatics::StaticLimenLog("FShaderCompiler::StartShaderBatching", CompilationMessage);
#endif
	
	FShaderPipelineCache::SetBatchMode(BatchMode);	
	FShaderPipelineCache::ResumeBatching();
	
	// OnCompilationStarted.Broadcast();
	CheckCompileFinished();
}

void FShaderCompiler::CheckCompileFinished()
{;
	CheckCompileFinishedDelegateHandle = FWorldDelegates::OnWorldTickStart.AddLambda([this] (UWorld*, ELevelTick, float)
	{
		CheckCompileFinished_Internal();
	});
}

void FShaderCompiler::CheckCompileFinished_Internal()
{
	bIsCompiling = true;
	if (TotalShaders == -1)
	{
		TotalShaders = FShaderPipelineCache::NumPrecompilesRemaining();
#if !UE_BUILD_SHIPPING
		const FString CompilationMessage = FString::Printf(TEXT("Compiling %lld shaders"), TotalShaders);
		ULimenCoreStatics::StaticLimenLog("FShaderCompiler::StartShaderBatching", CompilationMessage);
#endif
	}
	
	RemainingShaders = FShaderPipelineCache::NumPrecompilesRemaining();
	check(TotalShaders >= RemainingShaders);

	OnShaderCompilationUpdatedWithDetails.Broadcast(RemainingShaders, TotalShaders);

	if (PreviousRemainingShaders != RemainingShaders)
	{
		OnShaderCompilationUpdated.Broadcast(100 - RemainingShaders * 100 / TotalShaders);
		PreviousRemainingShaders = RemainingShaders;
	}
	
	if (RemainingShaders == 0 || bStopCompiling)
	{
		bIsCompiling = false;
		TotalShaders = 0;

		FShaderPipelineCache::PauseBatching();
			
		OnShaderCompilationUpdated.Broadcast(1);
		OnCompilationFinished.Broadcast();
		FWorldDelegates::OnWorldTickStart.Remove(CheckCompileFinishedDelegateHandle);
		TotalShaders = -1;
	}
}

FShaderCompiler::FShaderCompiler()
{
	World = nullptr;
	BatchMode = FShaderPipelineCache::BatchMode::Background;
	bIsCompiling = false;
	TotalShaders = -1;
	RemainingShaders = 0;
	PreviousRemainingShaders = 0;
	bStopCompiling = false;
}

ULimenGameInstance* ULimenGameInstance::GetGameInstance(UWorld* World)
{
	return GetGameInstance<ULimenGameInstance>(World);
}

void ULimenGameInstance::Init()
{
	Super::Init();
	InitializationProxy = nullptr;
	PSOBatch = FShaderCompiler::Get();
	WorldChangeCount = 0;
	
	// Shader compile Initialization Proxy
	PSOBatch->OnShaderCompilationUpdated.AddUObject(this, &ThisClass::ShaderCompilationProgressUpdated);
	PSOBatch->OnCompilationStarted.AddUObject(this, &ThisClass::PSOBatchStarted);
	PSOBatch->OnCompilationFinished.AddUObject(this, &ThisClass::PSOBatchFinished);

	FWorldDelegates::OnPostWorldInitialization.AddUObject(this, &ThisClass::PostWorldInitialization);
}

void ULimenGameInstance::Shutdown()
{
	Super::Shutdown();
}

const TSharedPtr<FShaderCompiler>& ULimenGameInstance::GetShaderCompiler()
{
	return PSOBatch;
}

void ULimenGameInstance::StartPreGameShaderCompile() const
{
	check(IsValid(GetWorld()))
	PSOBatch->StartCompiling(GetWorld(), FShaderPipelineCache::BatchMode::Precompile);
}

void ULimenGameInstance::StartLoadingScreenShaderCompile() const
{
	check(IsValid(GetWorld()))
	PSOBatch->StartCompiling(GetWorld(), FShaderPipelineCache::BatchMode::Background);
}

void ULimenGameInstance::StopShaderCompilation() const
{
	PSOBatch->StopCompiling();
}

void ULimenGameInstance::PostWorldInitialization(UWorld* World, const UWorld::InitializationValues IVS)
{
	if (WorldChangeCount == 0)
	{
		SetupInitialization();
	}
	
	WorldChangeCount++;	
}

void ULimenGameInstance::SetupInitialization()
{
#if !WITH_EDITOR
	BindLoadingScreenEventsDelegateHandle	= PSOBatch->OnCompilationFinished.AddUObject(this, &ThisClass::BindLoadingScreenEvents);
	OpenMainMenuDelegateHandle				= PSOBatch->OnCompilationFinished.AddUObject(this, &ULimenGameInstance::OpenMainMenu);

	StartPreGameShaderCompile();
#else
	BindLoadingScreenEvents();
#endif
}

void ULimenGameInstance::BindLoadingScreenEvents()
{
	PSOBatch->OnCompilationFinished.Remove(BindLoadingScreenEventsDelegateHandle);
	
	ULimenLevelTransitionSubsystem* LoadingScreenSubsystem = GetSubsystem<ULimenLevelTransitionSubsystem>();
	check(LoadingScreenSubsystem);
	LoadingScreenSubsystem->OnLoadingScreenVisible.AddUObject(this, &ThisClass::StartLoadingScreenShaderCompile);
}

void ULimenGameInstance::OpenMainMenu()
{
	PSOBatch->OnCompilationFinished.Remove(OpenMainMenuDelegateHandle);
	
	ULimenLevelManagerSubsystem* LevelManagerSubsystem = GetSubsystem<ULimenLevelManagerSubsystem>();
	LevelManagerSubsystem->OpenMainMenu();
}

void ULimenGameInstance::PSOBatchStarted()
{
	if (InitializationProxy == nullptr)
	{
		InitializationProxy = GetWorld()->SpawnActor<AInitializerProxyActor>();
	}
}

void ULimenGameInstance::PSOBatchFinished()
{
	if (InitializationProxy != nullptr)
	{
		verify(InitializationProxy->Destroy());
		InitializationProxy = nullptr;
	}
}

void ULimenGameInstance::ShaderCompilationProgressUpdated(const double Percentage)
{
	OnShaderCompilationProgressUpdated.Broadcast(Percentage);
}
