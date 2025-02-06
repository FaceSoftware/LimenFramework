// Copyright Face Software. All Rights Reserved.


#include "Subsystems/LimenLevelTransitionSubsystem.h"

#include "EngineUtils.h"
#include "ShaderPipelineCache.h"
#include "TimerManager.h"
#include "BlueprintLibraries/LimenCoreStatics.h"
#include "UMG/LimenLoadingScreenWidget.h"
#include "Developer/LimenLoadingScreenSettings.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Engine/GameViewportClient.h"
#include "Framework/Application/IInputProcessor.h"
#include "Framework/Application/SlateApplication.h"
#include "Objects/LimenLoadingScreenParameters.h"


class FLimenLoadingScreenInputPreProcessor final : public IInputProcessor
{
	virtual void Tick(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> Cursor) override {}

protected:
	bool ShouldAllowInput() { return false; }
};

ULimenLevelTransitionSubsystem::ULimenLevelTransitionSubsystem()
{
	bIsPreLoadingLevel = false;
	bIsLoadingScreenNotifiedToHide = false;
	TransientMasterVolumeCachedValue = 0;
	TotalPrecompiles = 0;
	CurrentPrecompileDonePercentage = 0;
}

bool ULimenLevelTransitionSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	if (!Super::ShouldCreateSubsystem(Outer))
	{
		return false;
	}

	const UGameInstance* GameInstance = CastChecked<UGameInstance>(Outer);
	return !GameInstance->IsDedicatedServerInstance();
}

void ULimenLevelTransitionSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	LoadingScreens = GetDefault<ULimenLoadingScreenSettings>()->LoadingScreens;
	
	FCoreUObjectDelegates::PreLoadMapWithContext.AddUObject(this, &ThisClass::HandlePreLoadMap);
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &ThisClass::HandlePostLoadMap);
}

void ULimenLevelTransitionSubsystem::Deinitialize()
{
	FCoreUObjectDelegates::PreLoadMapWithContext.RemoveAll(this);
	FCoreUObjectDelegates::PostLoadMapWithWorld.RemoveAll(this);
	
	LoadingScreenWidget = nullptr;

	Super::Deinitialize();
}

void ULimenLevelTransitionSubsystem::Tick(float DeltaTime)
{
	UpdateLoadingScreen(DeltaTime);
}

ETickableTickType ULimenLevelTransitionSubsystem::GetTickableTickType() const
{
	return ETickableTickType::Conditional;
}

bool ULimenLevelTransitionSubsystem::IsTickable() const
{
	return !HasAnyFlags(RF_ClassDefaultObject);
}

TStatId ULimenLevelTransitionSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(ULimenLevelTransitionSubsystem, STATGROUP_Tickables);
}

UWorld* ULimenLevelTransitionSubsystem::GetTickableGameObjectWorld() const
{
	return GetGameInstance()->GetWorld();
}

bool ULimenLevelTransitionSubsystem::IsLoadingScreenActive() const
{
	if (!LoadingScreenWidget)
	{
		return false;
	}

	return LoadingScreenWidget->IsShowing();
}

bool ULimenLevelTransitionSubsystem::PlayLoadingScreenForCurrentLevel()
{
	if (IsLoadingScreenActive())
	{
		HideLoadingScreen();
	}
	
	const FSoftObjectPath LevelPath = GetWorld();
	const FString MapName = UWorld::RemovePIEPrefix(LevelPath.GetAssetName());
	for (const auto& MapLoadingScreen : LoadingScreens)
	{
		if (MapLoadingScreen.Key.GetLongPackageName().Contains(MapName))
		{
			CurrentLoadingScreenSettings = MapLoadingScreen.Value.LoadSynchronous();
			if (CurrentLoadingScreenSettings == nullptr)
			{
				return false;
			}
			
			break;
		}
	}

	DisplayLoadingScreen();
	
	return true;
}

void ULimenLevelTransitionSubsystem::UpdateLoadingScreen(float DeltaTime)
{
	if (CurrentLoadingScreenSettings == nullptr)
	{
		return;
	}

	if (IsLoadingScreenActive())
	{
		SecondsShown += DeltaTime;

#if !WITH_EDITOR
		const uint32 ShadersLeft = FShaderPipelineCache::NumPrecompilesRemaining();
		if (FShaderPipelineCache::IsBatchingPaused() && ShadersLeft > 0)
		{
			// Somehow it pauses sometimes, even though resume was called...
			FShaderPipelineCache::ResumeBatching();
		}
			
		if (ShadersLeft > TotalPrecompiles)
		{
			TotalPrecompiles = ShadersLeft;
		}
			
		GEngine->AddOnScreenDebugMessage(FName(TEXT("ShaderPrecompilation")).ToUnstableInt(), 1.f, FColor::Cyan, FString::Printf(TEXT("Compiling %u shaders"), ShadersLeft));

		const float TempPercentageDone = 1.f - static_cast<float>(ShadersLeft) / static_cast<float>(TotalPrecompiles);
		CurrentPrecompileDonePercentage = TempPercentageDone < CurrentPrecompileDonePercentage ? CurrentPrecompileDonePercentage : TempPercentageDone;
		OnShaderCompilationUpdated.Broadcast(CurrentPrecompileDonePercentage);
#endif
	}
	
	if (ShouldShowLoadingScreen() && !IsLoadingScreenActive())
	{
		SecondsShown = 0;
		DisplayLoadingScreen();
		bIsLoadingScreenNotifiedToHide = false;
	}
	else if (!ShouldShowLoadingScreen() && IsLoadingScreenActive() && !bIsLoadingScreenNotifiedToHide)
	{
		bIsLoadingScreenNotifiedToHide = true;
		TotalPrecompiles = 0;
		if (!FShaderPipelineCache::IsBatchingPaused())
		{
			FShaderPipelineCache::PauseBatching();
		}
		HideLoadingScreen();
	}
}

void ULimenLevelTransitionSubsystem::DisplayLoadingScreen()
{
	if (CurrentLoadingScreenSettings == nullptr || CurrentLoadingScreenSettings->GetLoadingScreenWidgetClass() == nullptr)
	{
		return;
	}
	
	if (LoadingScreenWidget == nullptr)
	{
		UUserWidget* Instance = UUserWidget::CreateWidgetInstance(*GetGameInstance(), CurrentLoadingScreenSettings->GetLoadingScreenWidgetClass(), NAME_None);
		LoadingScreenWidget = Cast<ULimenLoadingScreenWidget>(Instance);
		check(IsValid(LoadingScreenWidget.Get()));
		LoadingScreenWidget->OnLimenAnimationFinished.AddUniqueDynamic(this, &ThisClass::LoadingScreenAnimationFinished);
	}
	
	LoadingScreenWidget->ShowWidget();
	FSlateApplication::Get().Tick();
	
	ChangePerformanceSettings(true);
	BlockInput();
	DisableAudio();

#if !WITH_EDITOR
	FShaderPipelineCache::PauseBatching();

	FShaderPipelineCache::SetBatchMode(FShaderPipelineCache::BatchMode::Precompile);
	
	TotalPrecompiles = FShaderPipelineCache::NumPrecompilesRemaining();
	ULimenCoreStatics::LimenLog(this, FString::Printf(TEXT("Compiling %u shaders"), TotalPrecompiles));

	if (TotalPrecompiles == 0)
	{
		OnShaderCompilationUpdated.Broadcast(1.f);
		CurrentPrecompileDonePercentage = 1.f;
	}
	else
	{
		OnShaderCompilationUpdated.Broadcast(0.f);
		CurrentPrecompileDonePercentage = 0.f;
		FShaderPipelineCache::ResumeBatching();
	}
#endif
	
	OnLoadingScreenVisible.Broadcast();
}

void ULimenLevelTransitionSubsystem::HideLoadingScreen()
{
	ChangePerformanceSettings(false);
	bIsLoadingScreenNotifiedToHide = true;
	if (LoadingScreenWidget)
	{
		LoadingScreenWidget->DestroyWidget(true);
		LoadingScreenWidget = nullptr;
	}

	UnblockInput();
	EnableAudio();

	CurrentLoadingScreenSettings = nullptr;

	OnLoadingScreenHidden.Broadcast();
	OnShaderCompilationUpdated.Broadcast(1.f);
}

bool ULimenLevelTransitionSubsystem::ShouldShowLoadingScreen() const
{
	bool bShouldShowLoadingScreen = false;
	bShouldShowLoadingScreen |= SecondsShown < CurrentLoadingScreenSettings->GetMinimumDisplayTimeSeconds();
	bShouldShowLoadingScreen |= bIsPreLoadingLevel;

	if (bShouldShowLoadingScreen)
	{
		return true;
	}

	if (FShaderPipelineCache::NumPrecompilesRemaining() > 0)
	{
		return true;
	}

	for (FActorIterator It(GetWorld()); It; ++It)
	{
		if (It->Implements<ULimenAsyncInitializer>() && ILimenAsyncInitializer::Execute_IsWorking(*It))
		{
			return true;
		}
	}
	
	return false;
}

void ULimenLevelTransitionSubsystem::HandlePreLoadMap(const FWorldContext& WorldContext, const FString& MapName)
{
	for (const TTuple<TSoftObjectPtr<UWorld>, TSoftObjectPtr<ULimenLoadingScreenParameters>>& MapLoadingScreen : LoadingScreens)
	{
		if (MapName.Contains(MapLoadingScreen.Key.GetLongPackageName()) &&
			MapName.Contains(MapLoadingScreen.Key.GetAssetName()))
		{
			CurrentLoadingScreenSettings = MapLoadingScreen.Value.LoadSynchronous();
			if (CurrentLoadingScreenSettings == nullptr)
			{
				return;
			}
			
			break;
		}
	}
	
	bIsPreLoadingLevel = true;
	if (GEngine->IsInitialized())
	{
		UpdateLoadingScreen(0);
	}
}

void ULimenLevelTransitionSubsystem::HandlePostLoadMap(UWorld* World)
{
	LoadedWorld = World;
	bIsPreLoadingLevel = false;
	GEngine->ForceGarbageCollection(true);
}

void ULimenLevelTransitionSubsystem::BlockInput()
{
	if (!InputPreProcessor.IsValid())
	{
		InputPreProcessor = MakeShared<FLimenLoadingScreenInputPreProcessor>();
		FSlateApplication::Get().RegisterInputPreProcessor(InputPreProcessor, 0);
	}
}

void ULimenLevelTransitionSubsystem::UnblockInput()
{
	if (InputPreProcessor.IsValid())
	{
		FSlateApplication::Get().UnregisterInputPreProcessor(InputPreProcessor);
		InputPreProcessor.Reset();
	}
}

void ULimenLevelTransitionSubsystem::ChangePerformanceSettings(const bool bEnablingLoadingScreen) const
{
	const UGameInstance* GameInstance = GetWorld()->GetGameInstance();
	UGameViewportClient* GameViewportClient = GameInstance->GetGameViewportClient();
	GameViewportClient->bDisableWorldRendering = bEnablingLoadingScreen;

	// Make sure to prioritize streaming in levels if the loading screen is up
	if (const UWorld* ViewportWorld = GameViewportClient->GetWorld())
	{
		if (AWorldSettings* WorldSettings = ViewportWorld->GetWorldSettings(false, false))
		{
			WorldSettings->bHighPriorityLoadingLocal = bEnablingLoadingScreen;
		}
	}
}

void ULimenLevelTransitionSubsystem::LoadingScreenAnimationFinished(const bool bIsVisibleAnimation)
{
	OnLoadingScreenVisibilityChanged.Broadcast(bIsVisibleAnimation);
}

void ULimenLevelTransitionSubsystem::EnableAudio()
{
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController(); PC != nullptr)
	{
		PC->ClearAudioListenerOverride();
	}
}

void ULimenLevelTransitionSubsystem::DisableAudio()
{
	const FVector& SilentLocation = GetDefault<ULimenLoadingScreenSettings>()->SilentLocation;
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController(); PC != nullptr)
	{
		PC->SetAudioListenerOverride(nullptr, SilentLocation, FRotator::ZeroRotator);
	}
}

AInitializerProxyActor* AInitializerProxyActor::CreateInitializerProxyActor(UObject* WorldContextObject)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (World == nullptr)
	{
		return nullptr;
	}

	AInitializerProxyActor* InitProxy = World->SpawnActor<AInitializerProxyActor>();
	check(InitProxy != nullptr);
	check(InitProxy->IsWorking())
	return InitProxy;
}

void AInitializerProxyActor::DestroyInitializerProxyActor(UObject* WorldContextObject,
	AInitializerProxyActor* InitProxyActor)
{
	if (InitProxyActor == nullptr)
	{
		return;
	}

	verify(InitProxyActor->Destroy());
}

AInitializerProxyActor::AInitializerProxyActor()
{
	bIsWorking = true;
}

void AInitializerProxyActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	bIsWorking = false;
	Super::EndPlay(EndPlayReason);
}

bool AInitializerProxyActor::IsWorking_Implementation() const
{
	return bIsWorking;
}
