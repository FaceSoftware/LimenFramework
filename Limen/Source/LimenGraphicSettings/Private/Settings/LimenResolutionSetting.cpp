// Copyright Face Software. All Rights Reserved.


#include "Settings/LimenResolutionSetting.h"

#include "DynamicRHI.h"
#include "TimerManager.h"
#include "UnrealEngine.h"
#include "BlueprintLibraries/LimenString.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Framework/Application/SlateApplication.h"
#include "GameFramework/GameUserSettings.h"
#include "Settings/LimenWindowModeSetting.h"
#include "Subsystems/LimenGraphicalSettingsSubsystem.h"
#include "Subsystems/LimenModalsSubsystem.h"
#include "Subsystems/LimenModularSettingsSubsystem.h"


const FString ULimenResolutionSetting::ConfirmationModalTitle = TEXT("New resolution");
const FString ULimenResolutionSetting::SecondsTag = TEXT("%SECONDS");
const FString ULimenResolutionSetting::ConfirmationModalText = TEXT("To prevent the UI screen from becoming unusable, "
																	"the settings will be reverted in " + SecondsTag +
																	" seconds. Click \"Ok\" if the current resolution"
																	" works for your monitor.");
const float ULimenResolutionSetting::ResolutionChangeTimeout = 10.f;

ULimenResolutionSetting::ULimenResolutionSetting(): bIsResolutionChangeConfirmed(false), bShouldTick(false),
													CurrentTime(0)
{
	DevelopmentName = TEXT("setting_resolution");
	Category = FText::FromString(TEXT("Display"));
	DisplayName = FText::FromString(TEXT("Resolution"));
	Description = FText::FromString(TEXT("Defines the display resolution, higher is better."));
}

void ULimenResolutionSetting::SubsystemInitialized(ULimenModularSettingsSubsystem* ModularSettingsSubsystem)
{
	Super::SubsystemInitialized(ModularSettingsSubsystem);

	ULimenSetting* WindowModeSetting = ModularSettingsSubsystem->GetItem<ULimenSetting>(ULimenWindowModeSetting::StaticClass());
	WindowModeSetting->OnSettingUpdated.AddUniqueDynamic(this, &ThisClass::OnWindowModeUpdated);
	
	ULimenSetting* MonitorDisplaySetting = ModularSettingsSubsystem->GetItem<ULimenSetting>(ULimenWindowModeSetting::StaticClass());
	MonitorDisplaySetting->OnSettingUpdated.AddUniqueDynamic(this, &ThisClass::OnMonitorDisplayUpdated);
}

bool ULimenResolutionSetting::CanEdit() const
{
	return CurrentWindowModeSetting.Compare(TEXT("Windowed Fullscreen")) != 0;
}

void ULimenResolutionSetting::Tick(float DeltaTime)
{	
	CurrentTime -= DeltaTime;
	if (bIsResolutionChangeConfirmed)
	{		
		ModalWidget->DestroyWidget();
		bShouldTick = false;
	}
	else if (CurrentTime < 0.f)
	{
		SetNewValue(GetPreviousValue());
		
		GEngine->GetGameUserSettings()->SetScreenResolution({DeFormatResolution(GetCurrentValue())});
		GEngine->GetGameUserSettings()->ApplySettings(false);
		OnSettingUpdated.Broadcast(this);
		
		ModalWidget->DestroyWidget();
		bShouldTick = false;
	}
	else
	{
		ModalWidget->SetParams({ConfirmationModalTitle, FormatModalText(static_cast<int32>(CurrentTime))});
	}
}

ETickableTickType ULimenResolutionSetting::GetTickableTickType() const
{
	return ETickableTickType::Conditional;
}

bool ULimenResolutionSetting::IsTickable() const
{
	return bShouldTick;
}

bool ULimenResolutionSetting::IsAllowedToTick() const
{
	return FTickableGameObject::IsAllowedToTick();
}

TStatId ULimenResolutionSetting::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(ULimenResolutionSetting, STATGROUP_Tickables);
}

bool ULimenResolutionSetting::IsTickableWhenPaused() const
{
	return true;
}

bool ULimenResolutionSetting::IsTickableInEditor() const
{
	return false;
}

UWorld* ULimenResolutionSetting::GetTickableGameObjectWorld() const
{
	return FTickableGameObject::GetTickableGameObjectWorld();
}

void ULimenResolutionSetting::ApplyCurrentSetting()
{	
	Super::ApplyCurrentSetting();

	if (GetCurrentValue() == GetPreviousValue() || GetCurrentValue() == DefaultSelection)
	{
		return;
	}
	
	GEngine->GetGameUserSettings()->SetScreenResolution(DeFormatResolution(GetCurrentValue()));
	GEngine->GetGameUserSettings()->ApplySettings(false);

	const ULimenModalsSubsystem* ModalsSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<ULimenModalsSubsystem>();
	ModalWidget = ModalsSubsystem->DisplayConfirmationModal({
		ConfirmationModalTitle, FormatModalText(10)
	});
	ModalWidget->OnModalResponseReceived.AddUniqueDynamic(this, &ThisClass::ResolutionConfirmationReceived);

	CurrentTime = ResolutionChangeTimeout;
	bIsResolutionChangeConfirmed = false;
	bShouldTick = true;
}

void ULimenResolutionSetting::SetDefaults()
{
	DefaultSelection = FormatResolution(GSystemResolution.ResX, GSystemResolution.ResY);
	PossibleSelections.Empty();

	if (FScreenResolutionArray SupportedResolutions;
		GDynamicRHI->RHIGetAvailableResolutions(SupportedResolutions, true))
	{
		for (FScreenResolutionRHI& SupportedResolution : SupportedResolutions)
		{
			PossibleSelections.Push(FormatResolution(SupportedResolution));
		}
	}
	else
	{
		PossibleSelections.Push(DefaultSelection);
	}
}

FString ULimenResolutionSetting::FormatResolution(const FScreenResolutionRHI& InResolution)
{
	return FString::Printf(TEXT("%dx%d"), InResolution.Width, InResolution.Height);
}

FString ULimenResolutionSetting::FormatResolution(const uint32 Width, const uint32 Height)
{
	return FString::Printf(TEXT("%dx%d"), Width, Height);
}

FIntPoint ULimenResolutionSetting::DeFormatResolution(const FString& InResolutionString)
{
	int32 SplitIndex; InResolutionString.FindChar(TEXT('x'), SplitIndex);
	FString Width, Height;
	ULimenString::SplitStringAtIndex(InResolutionString, SplitIndex, Width, Height);
	check(Width.IsNumeric());
	check(Height.IsNumeric());
	
	FIntPoint Res;
	Res.X = FCString::Atoi(*Width);
	Res.Y = FCString::Atoi(*Height);
	return Res;
}

void ULimenResolutionSetting::OnWindowModeUpdated(const ULimenSetting* Setting)
{
	const auto* WindowModeSetting = Cast<ULimenWindowModeSetting>(Setting);
	CurrentWindowModeSetting = WindowModeSetting->GetCurrentValue();
	
	if (CurrentWindowModeSetting.Compare(TEXT("Windowed Fullscreen")) == 0 ||
		LastWindowModeSetting.Compare(TEXT("Windowed Fullscreen")) == 0)
	{
		SetDefaultValue();
		ApplySetting();
		OnSettingEditableStateChanged.Broadcast(this);
	}

	LastWindowModeSetting = WindowModeSetting->GetCurrentValue();
}

void ULimenResolutionSetting::OnMonitorDisplayUpdated(const ULimenSetting* Setting)
{
	FScreenResolutionArray SupportedResolutions;
	PossibleSelections.Empty();

	if (GDynamicRHI->RHIGetAvailableResolutions(SupportedResolutions, true))
	{
		PossibleSelections.Reserve(1);
		PossibleSelections.Push(DefaultSelection);
		return;
	}
	
	PossibleSelections.Reserve(SupportedResolutions.Num());
	for (FScreenResolutionRHI& SupportedResolution : SupportedResolutions)
	{
		PossibleSelections.Push(FormatResolution(SupportedResolution));
	}
}

void ULimenResolutionSetting::ResolutionConfirmationReceived(ULimenGenericModalWidget* ULimenGenericModalWidget, const bool bConfirmed)
{
	bIsResolutionChangeConfirmed = true;
}

FString ULimenResolutionSetting::FormatModalText(const int32 SecondsRemaining)
{
	FString Out = ConfirmationModalText;
	FString SecondsString = FString::Printf(TEXT("%d"), SecondsRemaining);
	Out = Out.Replace(*SecondsTag, *SecondsString);
	return Out;
}
