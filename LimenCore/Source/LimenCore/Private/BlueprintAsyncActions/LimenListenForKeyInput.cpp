// Copyright FaceSoftware. All rights reserved.


#include "BlueprintAsyncActions/LimenListenForKeyInput.h"

#include "Engine/Engine.h"

ULimenListenForKeyInput* ULimenListenForKeyInput::ListenForKeyInput(UObject* WorldContext,
																	APlayerController* InPlayerController,
																	const FKey& InKey,
																	const float InTimeout,
																	const FListenForKeyInputDelegate OnKeyPressed,
																	const FListenForKeyInputDelegate OnWrongKeyPressed,
																	const FListenForKeyInputDelegate OnTimeout)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::ReturnNull);
	if (!World) return nullptr;

	auto* Instance = NewObject<ULimenListenForKeyInput>(WorldContext);
	Instance->Key = InKey;
	Instance->PlayerController = InPlayerController;
	Instance->Timeout = InTimeout;
	Instance->OnKeyPressed = OnKeyPressed;
	Instance->OnWrongKeyPressed = OnWrongKeyPressed;
	Instance->OnTimeout = OnTimeout;
	return Instance;
}

void ULimenListenForKeyInput::Activate()
{
	Super::Activate();

	bUseTimeout = Timeout > 0.f;
	WorldTickHandle = FWorldDelegates::OnWorldTickStart.AddUObject(this, &ThisClass::WorldTick);
}

void ULimenListenForKeyInput::WorldTick(UWorld* World, ELevelTick LevelTick, float DeltaTime)
{
	if (!PlayerController.IsValid())
	{
		FWorldDelegates::OnWorldTickStart.Remove(WorldTickHandle);
		return;
	}

	if (bUseTimeout)
	{
		Timeout -= DeltaTime;
		if (Timeout <= 0.f)
		{
			OnTimeout.ExecuteIfBound();
			FWorldDelegates::OnWorldTickStart.Remove(WorldTickHandle);
			return;
		}
	}
	if (PlayerController->IsInputKeyDown(Key))
	{
		OnKeyPressed.ExecuteIfBound();
		FWorldDelegates::OnWorldTickStart.Remove(WorldTickHandle);
	}
	else if (PlayerController->IsInputKeyDown(EKeys::AnyKey))
	{
		OnWrongKeyPressed.ExecuteIfBound();
		FWorldDelegates::OnWorldTickStart.Remove(WorldTickHandle);		
	}
}
