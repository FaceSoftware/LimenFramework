// Copyright Face Software. All Rights Reserved.


 #include "Items/LimenItemBase.h"

#include "TextureResource.h"
#include "TimerManager.h"
#include "Components/PrimitiveComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/Interactable/LimenInteractableAreaComponent.h"
#include "Engine/Engine.h"
#include "Engine/Texture2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "GameFramework/Pawn.h"
#include "ItemActions/LimenItemAction.h"
#include "Kismet/GameplayStatics.h"


UTexture* ALimenItemBase::GetItemImage(UObject* WorldContextObject, const TSubclassOf<ALimenItemBase>& ItemClass)
{
	check(ItemClass != nullptr);

	AActor* Actor = UGameplayStatics::GetActorOfClass(WorldContextObject, ItemClass);
	if (Actor == nullptr)
	{
		return nullptr;
	}

	ALimenItemBase* Item = CastChecked<ALimenItemBase>(Actor);
	Item->CaptureItemImage();
	return Item->GetItemImage();
}

FText ALimenItemBase::GetDisplayName(UObject* WorldContextObject, const TSubclassOf<ALimenItemBase>& ItemClass)
{
	check(ItemClass != nullptr);

	AActor* Actor = UGameplayStatics::GetActorOfClass(WorldContextObject, ItemClass);
	if (Actor == nullptr)
	{
		return FText();
	}

	return CastChecked<ALimenItemBase>(Actor)->GetDisplayName();
}

FText ALimenItemBase::GetDescription(UObject* WorldContextObject, const TSubclassOf<ALimenItemBase>& ItemClass)
{
	check(ItemClass != nullptr);

	AActor* Actor = UGameplayStatics::GetActorOfClass(WorldContextObject, ItemClass);
	if (Actor == nullptr)
	{
		return FText();
	}

	return CastChecked<ALimenItemBase>(Actor)->GetDescription();
}

FColor ALimenItemBase::GetRenderTargetBackgroundColor(UObject* WorldContextObject,
	const TSubclassOf<ALimenItemBase>& ItemClass)
{
	check(ItemClass != nullptr);

	AActor* Actor = UGameplayStatics::GetActorOfClass(WorldContextObject, ItemClass);
	if (Actor == nullptr)
	{
		return FColor::Transparent;
	}

	return CastChecked<ALimenItemBase>(Actor)->GetRenderTargetBackgroundColor();
}

ALimenItemBase::ALimenItemBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bHasBeenLoaded = false;
	RenderTargetBackgroundColor = FColor::Transparent;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	ItemQuantity = 1;

	ItemImageSceneCapture = CreateOptionalDefaultSubobject<USceneCaptureComponent2D>(TEXT("ItemImageSceneCapture"));
	bUseSceneCaptureForImage = ItemImageSceneCapture != nullptr;
	if (bUseSceneCaptureForImage)
	{
		ItemImageSceneCapture->SetupAttachment(GetRootComponent());
		ItemImageSceneCapture->bCaptureEveryFrame = false;
		ItemImageSceneCapture->bCaptureOnMovement = false;
		ItemImageSceneCapture->CaptureSource = ESceneCaptureSource::SCS_BaseColor;
		ItemImageSceneCapture->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_UseShowOnlyList;
	}
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

	if (bUseSceneCaptureForImage)
	{
		ItemImageRenderTarget2D = TStrongObjectPtr(NewObject<UTextureRenderTarget2D>());
		ItemImageRenderTarget2D->InitCustomFormat(1024, 1024, EPixelFormat::PF_FloatRGBA, true);
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

UStaticMesh* ALimenItemBase::GetItemMesh_Implementation() const
{
	return nullptr;
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

bool ALimenItemBase::ShouldSaveData() const
{
	return HasBeenInteracted();
}

bool ALimenItemBase::ShouldLoadData() const
{
	return bHasBeenLoaded;
}

void ALimenItemBase::DataLoaded()
{
	bHasBeenLoaded = true;
}

void ALimenItemBase::DataSaved()
{
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

	SetOwner(InPawn);

	for (ULimenInteractableAreaComponent*& InteractableComponent : GetInteractableComponents<ULimenInteractableAreaComponent>())
	{
		InteractableComponent->Deactivate();
	}

	if (FMath::IsNearlyZero(InteractAnimationTime))
	{
		RemoveFromGameplay();
	}
	else
	{
		InteractAnimation(InteractAnimationTime);
		GetWorld()->GetTimerManager().SetTimer(InteractAnimationTimerHandle, this, &ThisClass::RemoveFromGameplay, InteractAnimationTime, false);
	}
}

void ALimenItemBase::Drop(AController* InController, APawn* InPawn)
{
	check(HasAuthority())
	SetOwner(nullptr);

	for (ULimenInteractableAreaComponent*& InteractableComponent : GetInteractableComponents<ULimenInteractableAreaComponent>())
	{
		InteractableComponent->Activate(true);
	}

	AddToGameplay();
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
	if (bUseSceneCaptureForImage)
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
