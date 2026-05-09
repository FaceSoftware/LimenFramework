// Copyright Face Software. All Rights Reserved.


#include "Subsystems/LimenLevelTransitionSubsystem.h"

#include "EngineUtils.h"
#include "ShaderPipelineCache.h"
#include "UMG/LimenLoadingScreenWidget.h"
#include "Developer/LimenLoadingScreenSettings.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Engine/GameViewportClient.h"
#include "Engine/LevelStreaming.h"
#include "Framework/Application/IInputProcessor.h"
#include "Framework/Application/SlateApplication.h"
#include "Objects/LimenLoadingScreenParameters.h"


class FLimenLoadingScreenInputPreProcessor final : public IInputProcessor
{
public:
	virtual void Tick(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> Cursor) override {}

protected:
	constexpr bool ShouldAllowInput() { return false; }
};

ULimenLevelTransitionSubsystem::ULimenLevelTransitionSubsystem()
{
	bIsPreLoadingLevel = false;
	bIsLoadingScreenNotifiedToHide = false;
	TransientMasterVolumeCachedValue = 0;
	TotalPSOs = 0;
	PSOsLeft = 0;
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
	DefaultLoadingScreen = GetDefault<ULimenLoadingScreenSettings>()->DefaultLoadingScreen;
	
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
	if (!LoadingScreenWidget.IsValid()) { return false; }
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
			CurrentLoadingScreenSettings = TStrongObjectPtr(MapLoadingScreen.Value.LoadSynchronous());
			break;
		}
	}
	
	if (!CurrentLoadingScreenSettings.IsValid())
	{
		CurrentLoadingScreenSettings = TStrongObjectPtr(DefaultLoadingScreen.LoadSynchronous());
	}

	if (CurrentLoadingScreenSettings.IsValid())
	{
		DisplayLoadingScreen();
	}
	
	return CurrentLoadingScreenSettings.IsValid();
}

int32 ULimenLevelTransitionSubsystem::GetPSOsLeft() const
{
	return PSOsLeft;
}

int32 ULimenLevelTransitionSubsystem::GetTotalPSOs() const
{
	return TotalPSOs;
}

void ULimenLevelTransitionSubsystem::UpdateLoadingScreen(float DeltaTime)
{
	if (!CurrentLoadingScreenSettings.IsValid())
	{
		return;
	}

	if (IsLoadingScreenActive())
	{
		SecondsShown += DeltaTime;

#if !WITH_EDITOR
		if (uint32 NewPSOsLeft = FShaderPipelineCache::NumPrecompilesRemaining(); NewPSOsLeft != PSOsLeft)
		{
			PSOsLeft = NewPSOsLeft;
			if (PSOsLeft > TotalPSOs)
			{
				TotalPSOs = PSOsLeft;
			}
			OnPSOBatchingUpdated.Broadcast(TotalPSOs, PSOsLeft);
		}
		if (FShaderPipelineCache::IsBatchingPaused() && PSOsLeft > 0)
		{
			// Somehow it pauses sometimes, even though resume was called...
			FShaderPipelineCache::ResumeBatching();
		}
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
		TotalPSOs = 0;
#if !WITH_EDITOR
		if (!FShaderPipelineCache::IsBatchingPaused() && FShaderPipelineCache::NumPrecompilesRemaining() > 0)
		{
			FShaderPipelineCache::PauseBatching();
		}
#endif
		HideLoadingScreen();
	}
}

void ULimenLevelTransitionSubsystem::DisplayLoadingScreen()
{
	if (!CurrentLoadingScreenSettings.IsValid()|| !CurrentLoadingScreenSettings->GetLoadingScreenWidgetClass())
	{
		return;
	}
	
	if (!LoadingScreenWidget.IsValid())
	{
		UUserWidget* Instance = UUserWidget::CreateWidgetInstance(*GetGameInstance(), CurrentLoadingScreenSettings->GetLoadingScreenWidgetClass(), NAME_None);
		LoadingScreenWidget = CastChecked<ULimenLoadingScreenWidget>(Instance);

		LoadingScreenWidget->OnLimenVisibilityChanged.AddUObject(this, &ThisClass::LoadingScreenVisibilityChanged);
		LoadingScreenWidget->OnLimenAnimationFinished.AddUObject(this, &ThisClass::LoadingScreenAnimationFinished);
	}
	
	LoadingScreenWidget->ShowWidget();
	FSlateApplication::Get().Tick();
	
	ChangePerformanceSettings(true);
	BlockInput();
	DisableAudio();

#if !WITH_EDITOR
	FShaderPipelineCache::PauseBatching();
	FShaderPipelineCache::SetBatchMode(FShaderPipelineCache::BatchMode::Background);
	
	TotalPSOs = FShaderPipelineCache::NumPrecompilesRemaining();
	PSOsLeft = TotalPSOs;
	OnPSOBatchingUpdated.Broadcast(TotalPSOs, PSOsLeft);
#endif
}

void ULimenLevelTransitionSubsystem::HideLoadingScreen()
{
	ChangePerformanceSettings(false);
	bIsLoadingScreenNotifiedToHide = true;
	if (LoadingScreenWidget.IsValid())
	{
		LoadingScreenWidget->DestroyWidget(true);
		LoadingScreenWidget = nullptr;
	}

	UnblockInput();
	EnableAudio();

	CurrentLoadingScreenSettings.Reset();
	OnPSOBatchingUpdated.Broadcast(1.f, FShaderPipelineCache::NumPrecompilesRemaining());
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

#if !WITH_EDITOR
	if (FShaderPipelineCache::NumPrecompilesRemaining() > 0)
	{
		return true;
	}
#endif

	for (FActorIterator It(GetWorld()); It; ++It)
	{
		if (It->Implements<ULimenAsyncInitializer>() && ILimenAsyncInitializer::Execute_IsWorking(*It))
		{
			return true;
		}
	}

	for (const ULevelStreaming* StreamingLevel : GetWorld()->GetStreamingLevels())
	{
		if (StreamingLevel->IsStreamingStatePending())
		{
			return true;
		}
	}

	PostCompletedSecondsShown += GetWorld()->GetDeltaSeconds();
	if (PostCompletedSecondsShown < CurrentLoadingScreenSettings->GetPostCompleteDisplayTime())
	{
		return true;
	}

	PostCompletedSecondsShown = 0.f;
	return false;
}

void ULimenLevelTransitionSubsystem::HandlePreLoadMap(const FWorldContext& WorldContext, const FString& MapName)
{
	for (const TTuple<TSoftObjectPtr<UWorld>, TSoftObjectPtr<ULimenLoadingScreenParameters>>& MapLoadingScreen : LoadingScreens)
	{
		if (MapName.Contains(MapLoadingScreen.Key.GetLongPackageName()) &&
			MapName.Contains(MapLoadingScreen.Key.GetAssetName()))
		{
			CurrentLoadingScreenSettings = TStrongObjectPtr(MapLoadingScreen.Value.LoadSynchronous());
			break;
		}
	}
	
	if (!CurrentLoadingScreenSettings.IsValid())
	{
		CurrentLoadingScreenSettings = TStrongObjectPtr(DefaultLoadingScreen.LoadSynchronous());
	}
	if (!CurrentLoadingScreenSettings.IsValid())
	{
		return;
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

void ULimenLevelTransitionSubsystem::LoadingScreenVisibilityChanged(const bool bIsVisible)
{
	if (!bIsVisible)
	{
		OnLoadingScreenVisibilityChanged.Broadcast(false);
	}
}

void ULimenLevelTransitionSubsystem::LoadingScreenAnimationFinished(const bool bIsVisible)
{
	if (bIsVisible)
	{
		OnLoadingScreenVisibilityChanged.Broadcast(true);
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
