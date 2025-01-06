// Copyright Face Software. All Rights Reserved.


#include "Subsystems/LimenModalsSubsystem.h"

#include "Blueprint/UserWidget.h"
#include "Developer/LimenModalsDeveloperSettings.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"


ULimenGenericModalWidget* ULimenModalsSubsystem::DisplayConfirmationModal(const FModalParams& InParams) const
{
	const ULimenModalsDeveloperSettings* Settings = GetDefault<ULimenModalsDeveloperSettings>();
	check(Settings != nullptr);
	
	ULimenGenericModalWidget* ModalWidget = DisplayModalInternal(Settings->ConfirmationModalClass.LoadSynchronous(), InParams);
	return ModalWidget;
}

ULimenGenericModalWidget* ULimenModalsSubsystem::DisplayConsentModal(const FModalParams& InParams) const
{
	const ULimenModalsDeveloperSettings* Settings = GetDefault<ULimenModalsDeveloperSettings>();
	check(Settings != nullptr);
	
	ULimenGenericModalWidget* ModalWidget = DisplayModalInternal(Settings->ConsentModalClass.LoadSynchronous(), InParams);
	return ModalWidget;
}

ULimenGenericModalWidget* ULimenModalsSubsystem::DisplayModalInternal(const TSubclassOf<ULimenGenericModalWidget>& ModalClass, const FModalParams& InParams) const
{
	ULimenGenericModalWidget* ModalWidgetInstance = CreateWidget<ULimenGenericModalWidget>(GetWorld(), ModalClass);
	ModalWidgetInstance->HideWidget(); // For some reason the widget turn visible after it's instanced
	ModalWidgetInstance->ShowWidget();
	
	ModalWidgetInstance->SetParams(InParams);
	return ModalWidgetInstance;
}

UConfirmationModalAsyncAction* UConfirmationModalAsyncAction::DisplayConfirmationModal(
	const UObject* WorldContextObject, const FModalParams& InParams)
{
	UConfirmationModalAsyncAction* Instance = NewObject<UConfirmationModalAsyncAction>();
	Instance->World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::Assert);
	Instance->Params = InParams;
	return Instance;
}

void UConfirmationModalAsyncAction::Activate()
{
	Super::Activate();

	const ULimenModalsSubsystem* ModalSubsystem = World->GetGameInstance()->GetSubsystem<ULimenModalsSubsystem>();
	ULimenGenericModalWidget* ModalWidget = ModalSubsystem->DisplayConfirmationModal(Params);
	ModalWidget->OnModalResponseReceived.AddDynamic(this, &ThisClass::ModalDismissed);
}

void UConfirmationModalAsyncAction::ModalDismissed(ULimenGenericModalWidget* ModalWidget, const bool bAccepted)
{
	if (OnModalDismissed.IsBound())
	{
		OnModalDismissed.Broadcast(ModalWidget, true);
	}
}

UConsentModalAsyncAction* UConsentModalAsyncAction::DisplayConsentModal(const UObject* WorldContextObject,
	const FModalParams& InParams)
{
	UConsentModalAsyncAction* Instance = NewObject<UConsentModalAsyncAction>();
	Instance->World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::Assert);
	Instance->Params = InParams;
	return Instance;
}

void UConsentModalAsyncAction::Activate()
{
	Super::Activate();

	const ULimenModalsSubsystem* ModalSubsystem = World->GetGameInstance()->GetSubsystem<ULimenModalsSubsystem>();
	ULimenGenericModalWidget* ModalWidget = ModalSubsystem->DisplayConsentModal(Params);
	ModalWidget->OnModalResponseReceived.AddDynamic(this, &ThisClass::ModalDismissed);
}

void UConsentModalAsyncAction::ModalDismissed(ULimenGenericModalWidget* ModalWidget, const bool bAccepted)
{
	if (OnModalDismissed.IsBound())
	{
		OnModalDismissed.Broadcast(ModalWidget, bAccepted);
	}
}
