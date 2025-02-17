// Copyright Face Software. All Rights Reserved.


#include "Items/LimenItemBase.h"

#include "TextureResource.h"
#include "TimerManager.h"
#include "Components/PrimitiveComponent.h"
#include "Components/SceneCaptureComponent2D.h"
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

	return CastChecked<ALimenItemBase>(Actor)->GetItemImage();
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

	ItemImageRenderTarget2D = CreateDefaultSubobject<UTextureRenderTarget2D>(TEXT("ItemImageRenderTarget2D"));
	ItemImageRenderTarget2D->InitCustomFormat(1024, 1024, EPixelFormat::PF_FloatRGBA, true);
	ItemImageRenderTarget2D->ClearColor = RenderTargetBackgroundColor;
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

		ItemImage = ItemImageRenderTarget2D;
		ItemImageSceneCapture->TextureTarget = ItemImageRenderTarget2D;
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
		GetWorld()->GetTimerManager().SetTimerForNextTick([this]
		{
			 ItemImageSceneCapture->CaptureScene();
		});
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
	if (bUseSceneCaptureForImage && ItemImageSceneCapture != nullptr)
	{
		ItemImageSceneCapture->CaptureScene();
	}
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

const FColor& ALimenItemBase::GetRenderTargetBackgroundColor() const
{
	return RenderTargetBackgroundColor;
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
