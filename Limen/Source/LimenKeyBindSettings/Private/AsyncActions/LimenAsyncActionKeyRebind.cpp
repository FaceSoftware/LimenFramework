// Copyright © 2024 FaceSoftware. All rights reserved.


#include "AsyncActions/LimenAsyncActionKeyRebind.h"

#include "Components/InputComponent.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PlayerController/LimenPlayerControllerBase.h"
#include "Subsystems/LimenModalsSubsystem.h"
#include "UMG/LimenGenericModalWidget.h"


const FString ULimenAsyncActionKeyRebind::Title = TEXT("Rebind Key");
const FString ULimenAsyncActionKeyRebind::Content = TEXT("Press the new desired key. This will timeout in #TIMEREMAINING seconds");
const FString ULimenAsyncActionKeyRebind::TimeRemainingKeyword = TEXT("#TIMEREMAINING");

ULimenAsyncActionKeyRebind::ULimenAsyncActionKeyRebind(): TimeoutTime(0), ModalParams(Title, Content),
														  PreviousInputMode(ELimenInputMode::Undefined),
														  bShouldTick(false), TotalTickTime(0), SecondCounter(0)
{
}

void ULimenAsyncActionKeyRebind::BeginDestroy()
{
	ResetStates(true);
	OnKeyPressed.Clear();
	
	Super::BeginDestroy();
}

void ULimenAsyncActionKeyRebind::Activate()
{
	Super::Activate();
	
	const UWorld* World = GEngine->GetWorldFromContextObject(WorldContext.Get(), EGetWorldErrorMode::Assert);
	if (PlayerController == nullptr)
	{
		return;
	}

	PreviousInputMode = PlayerController->GetInputMode();
	PlayerController->SetInputMode(ELimenInputMode::Game);
	PlayerController->DisableInput(PlayerController.Get());

	ULimenModalsSubsystem* ModalsSubsystem = World->GetGameInstance()->GetSubsystem<ULimenModalsSubsystem>();
	DisplayModal = ModalsSubsystem->DisplayTimedModal(ModalParams);
	UpdateModal();

	InputListener = PlayerController->GetWorld()->SpawnActor<ALimenInputListener>();
	InputListener->EnableInput(PlayerController.Get());
	InputListener->OnKeyPressed.BindLambda([this] (const FKey& PressedKey)
	{
		bShouldTick = false;
		ResetStates(false);
		KeyMapping.Key = PressedKey;
		OnKeyPressed.Broadcast(KeyMapping);
		ConditionalBeginDestroy();
	});
	
	TotalTickTime = 0;
	SecondCounter = 0;
	bShouldTick = true;
}

ULimenAsyncActionKeyRebind* ULimenAsyncActionKeyRebind::RebindKey(UObject* InWorldContext,
	ALimenPlayerControllerBase* PlayerController, FEnhancedActionKeyMapping KeyMapping, const float InTimeoutTime)
{
	ULimenAsyncActionKeyRebind* Instance = NewObject<ULimenAsyncActionKeyRebind>();
	Instance->WorldContext = InWorldContext;
	Instance->PlayerController = PlayerController;
	Instance->KeyMapping = KeyMapping;
	Instance->TimeoutTime = InTimeoutTime;
	return Instance;
}

bool ULimenAsyncActionKeyRebind::IsTickableWhenPaused() const
{
	return FTickableGameObject::IsTickableWhenPaused();
}

bool ULimenAsyncActionKeyRebind::IsTickableInEditor() const
{
	return FTickableGameObject::IsTickableInEditor();
}

UWorld* ULimenAsyncActionKeyRebind::GetTickableGameObjectWorld() const
{
	return FTickableGameObject::GetTickableGameObjectWorld();
}

void ULimenAsyncActionKeyRebind::Tick(const float DeltaTime)
{
	TotalTickTime += DeltaTime;
	SecondCounter += DeltaTime;
	
	if (SecondCounter >= 1.f)
	{
		UpdateModal();
		SecondCounter = 0;
	}
	
	if (TotalTickTime >= TimeoutTime)
	{
		bShouldTick = false;
		TimedOut();
	}
}

ETickableTickType ULimenAsyncActionKeyRebind::GetTickableTickType() const
{
	return ETickableTickType::Conditional;
}

bool ULimenAsyncActionKeyRebind::IsTickable() const
{
	return bShouldTick;
}

TStatId ULimenAsyncActionKeyRebind::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(ULimenAsyncActionKeyRebind, STATGROUP_Tickables);
}

void ULimenAsyncActionKeyRebind::ResetStates(const bool bIsBeingDestroyed) const
{	
	if (InputListener.IsValid())
	{
		if (PlayerController != nullptr)
		{
			InputListener->DisableInput(PlayerController.Get());
		}
		verify(InputListener->Destroy());
	}
	if (PlayerController.IsValid())
	{
		PlayerController->EnableInput(PlayerController.Get());
		PlayerController->SetInputMode(PreviousInputMode);
	}
	if (DisplayModal != nullptr)
	{
		DisplayModal->DestroyWidget(!bIsBeingDestroyed);
	}
}

void ULimenAsyncActionKeyRebind::TimedOut()
{
	ResetStates(false);
	OnKeyPressed.Clear();
	ConditionalBeginDestroy();
}

void ULimenAsyncActionKeyRebind::UpdateModal()
{
	const int32 RemainingTime = TimeoutTime - TotalTickTime;
	const FString NewContent = Content.Replace(*TimeRemainingKeyword, *FString::Printf(TEXT("%d"), RemainingTime));

	ModalParams.Content = FText::FromString(NewContent);
	DisplayModal->SetParams(ModalParams);
}

void ALimenInputListener::EnableInput(APlayerController* PlayerController)
{
	Super::EnableInput(PlayerController);

	check(InputComponent)
	FInputKeyBinding& Binding = InputComponent->BindKey(EKeys::AnyKey, EInputEvent::IE_Pressed, this, &ThisClass::KeyPressed);
	Binding.bExecuteWhenPaused = true;
}

void ALimenInputListener::KeyPressed(FKey PressedKey)
{
	if (OnKeyPressed.IsBound())
	{
		OnKeyPressed.Execute(PressedKey);
	}
}
