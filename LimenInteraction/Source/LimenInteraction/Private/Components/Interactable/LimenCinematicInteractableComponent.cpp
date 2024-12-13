// Copyright Face Software. All Rights Reserved.


#include "Components/Interactable/LimenCinematicInteractableComponent.h"

#include "Actors/LimenGameplayActor.h"
#include "Camera/CameraComponent.h"


const FAttachmentTransformRules ULimenCinematicInteractableComponent::CameraAttachmentRules = FAttachmentTransformRules(
	EAttachmentRule::KeepRelative,
	EAttachmentRule::KeepRelative,
	EAttachmentRule::KeepWorld,
	false);

const FAttachmentTransformRules ULimenCinematicInteractableComponent::ItemSocketAttachmentRules = FAttachmentTransformRules(
	EAttachmentRule::KeepRelative,
	EAttachmentRule::KeepRelative,
	EAttachmentRule::KeepRelative,
	false);

ULimenCinematicInteractableComponent::ULimenCinematicInteractableComponent() : Super()
{
	CinematicCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("CinematicCamera"));
	
	ItemSocket = CreateDefaultSubobject<USceneComponent>(TEXT("ItemSocket"));
	
	TransitionParameters.BlendExp = 1.f;
	TransitionParameters.BlendFunction = VTBlend_EaseIn;
	TransitionParameters.BlendTime = 3.f;
	TransitionParameters.bLockOutgoing = false;
}

void ULimenCinematicInteractableComponent::AttachSubComponents()
{
	ItemSocket->SetupAttachment(this);
	CinematicCamera->SetupAttachment(this);
}

void ULimenCinematicInteractableComponent::OnComponentCreated()
{
	Super::OnComponentCreated();

	CinematicCamera->AttachToComponent(this, CameraAttachmentRules);
	CinematicCamera->SetWorldScale3D(FVector::OneVector);
	CinematicCamera->SetAutoActivate(false);
	
	ItemSocket->AttachToComponent(this, ItemSocketAttachmentRules);
	ItemSocket->SetWorldScale3D(FVector::OneVector);
}

#if WITH_EDITOR
void ULimenCinematicInteractableComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	const FName PropertyName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	if (PropertyName == NAME_None)
	{
		return;
	}

	if (PropertyName == GET_MEMBER_NAME_CHECKED(ThisClass, CinematicCameraOffset))
	{
		CinematicCamera->SetRelativeTransform(CinematicCameraOffset);
	}
	else if (PropertyName == GET_MEMBER_NAME_CHECKED(ThisClass, ItemSocketTransform))
	{
		ItemSocket->SetRelativeTransform(ItemSocketTransform);
	}
}
#endif WITH_EDITOR

void ULimenCinematicInteractableComponent::AddItem(ALimenGameplayActor* Item)
{
	if (Item == nullptr)
	{
		return;
	}
	
	ItemActor = Item;	
	verify(ItemActor->AttachToComponent(ItemSocket.Get(),
										FAttachmentTransformRules::SnapToTargetNotIncludingScale));

	verify(ItemActor->TeleportTo(ItemSocket->GetComponentLocation(),
							 	 ItemSocket->GetComponentRotation(),
							 	 false,
							 	 true));
}

void ULimenCinematicInteractableComponent::SetItemRelativeTransform(const FTransform& NewTransform)
{
	ItemSocketTransform = NewTransform;
	ItemSocket->SetRelativeTransform(ItemSocketTransform);
}

void ULimenCinematicInteractableComponent::Interact(AController* InController, APawn* InPawn)
{	
	PlayerController = Cast<APlayerController>(InController);
	PlayerPawn = InPawn;
	
	if (PlayerController.Get() == nullptr)
	{
		Super::Interact(InController, InPawn);
	}
	
	OnCinematicTriggered.Broadcast(PlayerController.Get(), PlayerPawn.Get());
}

void ULimenCinematicInteractableComponent::NotifyInteract()
{
	check(PlayerController != nullptr);
	Super::Interact(PlayerController.Get(), PlayerPawn.Get());
}

void ULimenCinematicInteractableComponent::StartCinematic(APlayerController* InPlayerController, APawn* InPlayerPawn)
{
	check(InPlayerController != nullptr);
	check(ItemActor != nullptr);
	
	PlayerOriginalViewTarget = PlayerController->GetViewTarget();

	OnCinematicStarted();
	OnCinematicStart.Broadcast(PlayerController.Get(), PlayerPawn.Get());
}

void ULimenCinematicInteractableComponent::StopCinematic()
{
	PlayerController->SetViewTarget(PlayerOriginalViewTarget.Get(), TransitionParameters);
	
	if (FMath::IsNegativeOrNegativeZero(TransitionParameters.BlendTime) ||
		FMath::IsNearlyZero(TransitionParameters.BlendTime))
	{
		OnCinematicStopped();
		OnCinematicEnd.Broadcast(PlayerController.Get(), PlayerPawn.Get());

		PlayerController.Reset();
		PlayerPawn.Reset();
		PlayerOriginalViewTarget.Reset();
		ItemActor.Reset();
	}
	else
	{
		FTimerHandle Handle;
		GetWorld()->GetTimerManager().SetTimer(Handle, [this]
		{
			OnCinematicStopped();
			OnCinematicEnd.Broadcast(PlayerController.Get(), PlayerPawn.Get());
			
			PlayerController.Reset();
			PlayerPawn.Reset();
			PlayerOriginalViewTarget.Reset();
			ItemActor.Reset();
		},
		TransitionParameters.BlendTime,
		false);
	}
}

APlayerController* ULimenCinematicInteractableComponent::GetCurrentPlayerController() const
{
	return PlayerController.Get();
}

APawn* ULimenCinematicInteractableComponent::GetCurrentPawn() const
{
	return PlayerPawn.Get();
}

ALimenGameplayActor* ULimenCinematicInteractableComponent::GetItemActor() const
{
	return ItemActor.Get();
}

void ULimenCinematicInteractableComponent::OnCinematicStarted()
{
	CinematicCamera->Activate(true);
	PlayerPawn->DisableInput(PlayerController.Get());
	PlayerController->FlushPressedKeys();
	PlayerController->SetViewTarget(GetOwner(), TransitionParameters);
	ItemActor->AddToGameplay();
}

void ULimenCinematicInteractableComponent::OnCinematicStopped()
{
	CinematicCamera->Deactivate();
	PlayerPawn->EnableInput(PlayerController.Get());
	ItemActor->RemoveFromGameplay();
}
