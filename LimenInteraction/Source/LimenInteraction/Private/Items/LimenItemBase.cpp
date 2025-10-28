// Copyright Face Software. All Rights Reserved.


#include "Items/LimenItemBase.h"

#include "TextureResource.h"
#include "TimerManager.h"
#include "Components/PrimitiveComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/Interactable/LimenInteractableAreaComponent.h"
#include "Engine/Engine.h"
#include "Engine/TextureRenderTarget2D.h"
#include "GameFramework/Pawn.h"
#include "ItemActions/LimenItemAction.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"


ALimenItemBase::ALimenItemBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	RenderTargetBackgroundColor = FColor::Transparent;
	ItemQuantity = 1;
	RenderTargetResolution = { 512, 512 };
	RenderTargetPixelFormat = EPixelFormat::PF_R8G8B8A8;
	bForceRenderTargetLinearGamma = false;

	ItemImageSceneCapture = CreateOptionalDefaultSubobject<USceneCaptureComponent2D>(ItemImageSceneCaptureName);
	bUseSceneCaptureForImage = ItemImageSceneCapture != nullptr;
	if (bUseSceneCaptureForImage)
	{
		ItemImageSceneCapture->SetupAttachment(GetRootComponent());
		ItemImageSceneCapture->SetMobility(EComponentMobility::Movable);
		ItemImageSceneCapture->bCaptureEveryFrame = false;
		ItemImageSceneCapture->bCaptureOnMovement = false;
		ItemImageSceneCapture->CaptureSource = ESceneCaptureSource::SCS_BaseColor;
		ItemImageSceneCapture->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_UseShowOnlyList;
	}
}

void ALimenItemBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;

	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, PickUpState, Params);
}

void ALimenItemBase::BeginPlay()
{
	Super::BeginPlay();
	
	for (const TSubclassOf<ULimenItemAction>& ActionClass : ItemActionsClass)
	{
		if (!ensureAlwaysMsgf(ActionClass != nullptr, TEXT("This item contains an invalid item action. Please remove it to prevent unnecessary overhead!!")))
		{
			continue;
		}
		
		ULimenItemAction* Action = NewObject<ULimenItemAction>(this, ActionClass);
		Action->SetupAction(this);
		ItemActions.Push(TStrongObjectPtr(Action));
	}

	if (bUseSceneCaptureForImage && ItemImageSceneCapture)
	{
		ItemImageRenderTarget2D = TStrongObjectPtr(NewObject<UTextureRenderTarget2D>());
		ItemImageRenderTarget2D->InitCustomFormat(RenderTargetResolution.X, RenderTargetResolution.Y, RenderTargetPixelFormat, bForceRenderTargetLinearGamma);
		ItemImageRenderTarget2D->ClearColor = RenderTargetBackgroundColor;

		if (!ItemImageSceneCapture->ShowOnlyActors.Contains(TObjectPtr<AActor>(this)))
		{
			ItemImageSceneCapture->ShowOnlyActors.Push(TObjectPtr<AActor>(this));
		}

		ItemImage = ItemImageRenderTarget2D.Get();
		ItemImageSceneCapture->TextureTarget = ItemImageRenderTarget2D.Get();
	}
}

void ALimenItemBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (ItemImageRenderTarget2D.IsValid())
	{
		ItemImageRenderTarget2D.Reset();
	}

	ItemActions.Empty();
	Super::EndPlay(EndPlayReason);
}

UStaticMeshComponent* ALimenItemBase::GetStaticMesh_Implementation() const
{
	return nullptr;
}

USkeletalMeshComponent* ALimenItemBase::GetSkeletalMesh_Implementation() const
{
	return nullptr;
}

UMeshComponent* ALimenItemBase::GetMesh_Implementation() const
{
	UMeshComponent* SkeletalMesh = GetSkeletalMesh();
	UMeshComponent* StaticMesh = GetStaticMesh();

	if (SkeletalMesh && StaticMesh) return StaticMesh;
	if (StaticMesh) return StaticMesh;
	if (SkeletalMesh) return SkeletalMesh;
	return nullptr;
}

void ALimenItemBase::OnRep_PickUpState()
{
	if (PickUpState.bIsDropped)
	{
		Dropped(PickUpState.InController.Get(), PickUpState.InPawn.Get());
		OnDropped.Broadcast();
	}
	else
	{
		PickedUp(PickUpState.InController.Get(), PickUpState.InPawn.Get());
		OnPickedUp.Broadcast();
	}
}

UTexture* ALimenItemBase::GetItemImage() const
{
	return ItemImage.Get();
}

const FText& ALimenItemBase::GetDisplayName() const
{
	return DisplayName;
}

const FText& ALimenItemBase::GetDescription() const
{
	return Description;
}

TArray<ULimenItemAction*> ALimenItemBase::GetItemActions() const
{
	TArray<ULimenItemAction*> Out;
	Out.Reserve(ItemActions.Num());
	
	for (auto& Action : ItemActions)
	{
		Out.Push(Action.Get());
	}
	
	return Out;
}

void ALimenItemBase::PickUp(AController* InController, APawn* InPawn)
{
	check(HasAuthority())

	PickUpState.InController = InController;
	PickUpState.InPawn = InPawn;
	PickUpState.bIsDropped = false;
	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, PickUpState, this)

	PickedUp(InController, InPawn);
	OnPickedUp.Broadcast();
}

void ALimenItemBase::Drop(AController* InController, APawn* InPawn)
{
	check(HasAuthority())

	PickUpState.InController = InController;
	PickUpState.InPawn = InPawn;
	PickUpState.bIsDropped = true;
	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, PickUpState, this)

	Dropped(InController, InPawn);
	OnDropped.Broadcast();
}

bool ALimenItemBase::IsDropped() const
{
	return PickUpState.bIsDropped;
}

int32 ALimenItemBase::GetItemQuantity() const
{
	return ItemQuantity;
}

void ALimenItemBase::SetItemQuantity(const int32 InItemQuantity)
{
	ItemQuantity = InItemQuantity;
}

const FColor& ALimenItemBase::GetRenderTargetBackgroundColor() const
{
	return RenderTargetBackgroundColor;
}

void ALimenItemBase::CaptureItemImage()
{
	if (bUseSceneCaptureForImage && ItemImageSceneCapture)
	{
		const bool bShouldHide = IsHidden();
		SetActorHiddenInGame(false);
		ItemImageSceneCapture->CaptureScene();
		SetActorHiddenInGame(bShouldHide);
	}
}

void ALimenItemBase::Interact(AController* InController, APawn* InPawn)
{
	Super::Interact(InController, InPawn);
}

void ALimenItemBase::InteractionStopped(AController* InController, APawn* InPawn)
{
	Super::InteractionStopped(InController, InPawn);
	
	// Hardcoded: Marked final because items should only need one interaction.
	// Remove the finality of this function in case that functionality is needed.
}

void ALimenItemBase::PickedUp(AController* InController, APawn* InPawn)
{
	if (HasAuthority()) { SetOwner(InPawn); }

	for (ULimenInteractableAreaComponent*& InteractableComponent : GetInteractableComponents<ULimenInteractableAreaComponent>())
	{
		InteractableComponent->Deactivate();
	}

	if (FMath::IsNearlyZero(PickupAnimationTime))
	{
		RemoveFromGameplay();
	}
	else
	{
		PickUpAnimation(PickupAnimationTime);
		GetWorld()->GetTimerManager().SetTimer(InteractAnimationTimerHandle,
											   this, &ThisClass::RemoveFromGameplay,
											   PickupAnimationTime, false);
	}
}

void ALimenItemBase::Dropped(AController* InController, APawn* InPawn)
{
	if (HasAuthority()) { SetOwner(nullptr); }

	for (ULimenInteractableAreaComponent*& InteractableComponent : GetInteractableComponents<ULimenInteractableAreaComponent>())
	{
		InteractableComponent->Activate(true);
	}

	AddToGameplay();
}

void ALimenItemBase::PickUpAnimation(const float AnimationTime)
{
}
