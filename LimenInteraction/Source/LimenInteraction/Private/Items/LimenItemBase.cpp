// Copyright Face Software. All Rights Reserved.


#include "Items/LimenItemBase.h"

#include "TextureResource.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/Texture2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "GameFramework/Pawn.h"
#include "ItemActions/LimenItemAction.h"


UTexture* ALimenItemBase::GetItemImage(const TSubclassOf<ALimenItemBase>& ItemClass)
{
	check(ItemClass != nullptr);
	return ItemClass->GetDefaultObject<ALimenItemBase>()->GetItemImage();
}

FText ALimenItemBase::GetDisplayName(const TSubclassOf<ALimenItemBase>& ItemClass)
{
	check(ItemClass != nullptr);
	return ItemClass->GetDefaultObject<ALimenItemBase>()->GetDisplayName();
}

FText ALimenItemBase::GetDescription(const TSubclassOf<ALimenItemBase>& ItemClass)
{
	check(ItemClass != nullptr);
	return ItemClass->GetDefaultObject<ALimenItemBase>()->GetDescription();
}

ALimenItemBase::ALimenItemBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bHasBeenLoaded = false;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

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

void ALimenItemBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (bUseSceneCaptureForImage)
	{
		if (!ItemImageSceneCapture->ShowOnlyActors.Contains(TObjectPtr<AActor>(this)))
		{
			ItemImageSceneCapture->ShowOnlyActors.Push(TObjectPtr<AActor>(this));
		}

		if (ItemImageRenderTarget == nullptr)
		{
			ItemImageRenderTarget = NewObject<UTextureRenderTarget2D>(this);
			ItemImageRenderTarget->InitCustomFormat(2048, 2048, EPixelFormat::PF_FloatRGBA, false);
		}
		ItemImageSceneCapture->TextureTarget = ItemImageRenderTarget.Get();

		ItemImage = ItemImageSceneCapture->TextureTarget.Get();

		ItemImageSceneCapture->CaptureScene();
		GetClass()->GetDefaultObject<ALimenItemBase>()->ItemImage = ItemImage;
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
}

void ALimenItemBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
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

void ALimenItemBase::Interact(AController* InController, APawn* InPawn)
{
	Super::Interact(InController, InPawn);

	SetOwner(InPawn);
	RemoveFromGameplay();
}

void ALimenItemBase::InteractionStopped(AController* InController, APawn* InPawn)
{
	Super::InteractionStopped(InController, InPawn);
	
	// Hardcoded: Marked final because items should only need one interaction.
	// Remove the finality of this function in case that functionality is needed.
}
