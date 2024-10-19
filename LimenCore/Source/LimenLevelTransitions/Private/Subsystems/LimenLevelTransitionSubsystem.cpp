// Copyright Face Software. All Rights Reserved.


#include "Subsystems/LimenLevelTransitionSubsystem.h"

#include "EngineUtils.h"
#include "UMG/LimenLoadingScreenWidget.h"
#include "Developer/LimenLoadingScreenSettings.h"
#include "Framework/Application/IInputProcessor.h"
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

void ULimenLevelTransitionSubsystem::UpdateLoadingScreen(float DeltaTime)
{
	if (CurrentLoadingScreenSettings == nullptr)
	{
		return;
	}
	
	if (ShouldShowLoadingScreen())
	{
		if (IsLoadingScreenActive())
		{
			SecondsShown += DeltaTime;
			return;
		}

		SecondsShown = 0;
		DisplayLoadingScreen();
		bIsLoadingScreenNotifiedToHide = false;
	}
	else
	{
		if (!bIsLoadingScreenNotifiedToHide)
		{
			HideLoadingScreen();
		}
	}
}

void ULimenLevelTransitionSubsystem::DisplayLoadingScreen()
{
	if (CurrentLoadingScreenSettings == nullptr || CurrentLoadingScreenSettings->GetLoadingScreenWidgetClass() == nullptr)
	{
		return;
	}
	
	if (!LoadingScreenWidget)
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
	
	OnLoadingScreenVisible.Broadcast();
}

void ULimenLevelTransitionSubsystem::HideLoadingScreen()
{
	ChangePerformanceSettings(false);
	bIsLoadingScreenNotifiedToHide = true;
	GetGameInstance()->GetTimerManager().SetTimer(RemoveLoadingScreenTimerHandle, this,
												  &ThisClass::HideLoadingScreen_Internal,
												  CurrentLoadingScreenSettings->HideLoadingScreenAnimationTime,
												  false);

	EnableAudio();
	CurrentLoadingScreenSettings = nullptr;
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
	for (auto& MapLoadingScreen : LoadingScreens)
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

void ULimenLevelTransitionSubsystem::HideLoadingScreen_Internal()
{
	if (LoadingScreenWidget)
	{
		LoadingScreenWidget->DestroyWidget();
		LoadingScreenWidget = nullptr;
	}
		
	UnblockInput();
	GetWorld()->GetTimerManager().ClearTimer(RemoveLoadingScreenTimerHandle);
	OnLoadingScreenHidden.Broadcast();
}

void ULimenLevelTransitionSubsystem::LoadingScreenAnimationFinished(const bool bIsVisibleAnimation)
{
	OnLoadingScreenVisibilityChanged.Broadcast(bIsVisibleAnimation);
}

void ULimenLevelTransitionSubsystem::EnableAudio()
{
	FAudioDeviceHandle AudioDevice = GEngine->GetMainAudioDevice();
	if (!AudioDevice.IsValid())
	{
		return;
	}

	// AudioDevice->SetTransientPrimaryVolume(static_cast<float>(TransientMasterVolumeCachedValue));
}

void ULimenLevelTransitionSubsystem::DisableAudio()
{
	FAudioDeviceHandle AudioDevice = GEngine->GetMainAudioDevice();
	if (!AudioDevice.IsValid())
	{
		return;
	}

	// TransientMasterVolumeCachedValue = AudioDevice->GetTransientPrimaryVolume();
	// AudioDevice->SetTransientPrimaryVolume(0);
}

bool AInitializerProxyActor::IsWorking_Implementation() const
{
	return true;
}
