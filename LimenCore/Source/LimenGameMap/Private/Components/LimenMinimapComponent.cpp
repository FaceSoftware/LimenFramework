// Copyright FaceSoftware. All rights reserved.


#include "Components/LimenMinimapComponent.h"

#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "GameFramework/Actor.h"
#include "GameFramework/SpringArmComponent.h"
#include "Materials/MaterialInstanceDynamic.h"


ULimenMinimapComponent::ULimenMinimapComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	MaterialParameter = TEXT("Texture");
	bWantsInitializeComponent = true;
	MiniMapRootOffset = FVector(0.f, 0.f, 500.f);

	SceneCaptureComponent =	TStrongObjectPtr(CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SceneCaptureComponent")));
}

void ULimenMinimapComponent::InitializeComponent()
{
	Super::InitializeComponent();

	if (!RenderTargetMaterial.IsNull())
	{
		auto* Instance = UMaterialInstanceDynamic::Create(RenderTargetMaterial.LoadSynchronous(),
										 				  this,
										 				  TEXT("MiniMapMaterial"));

		RenderTargetMaterialInstance = TStrongObjectPtr(Instance);
		RenderTargetMaterialInstance->SetTextureParameterValue(MaterialParameter, SceneCaptureComponent->TextureTarget.Get());
	}

	if (GetOwner())
	{
		FVector Origin, Extent;
		GetOwner()->GetActorBounds(true, Origin, Extent, false);
		OwnerHalfHeight = Extent.Z;

		SceneCaptureComponent->AttachToComponent(GetOwner()->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	}
	SceneCaptureComponent->SetRelativeRotation(FVector::DownVector.Rotation());
	SceneCaptureComponent->ProjectionType = ECameraProjectionMode::Orthographic;
	SceneCaptureComponent->CaptureSource = SCS_BaseColor;
	SceneCaptureComponent->TextureTarget = MiniMapRenderTarget.Get();
	SceneCaptureComponent->ClipPlaneNormal = FVector::DownVector;
	SceneCaptureComponent->ClipPlaneBase = FVector::ZeroVector;
	SceneCaptureComponent->ClipPlaneBase.Z = MiniMapRootOffset.Z - OwnerHalfHeight;
}

void ULimenMinimapComponent::UninitializeComponent()
{
	Super::UninitializeComponent();

	RenderTargetMaterialInstance.Reset();
	SceneCaptureComponent.Reset();
}

void ULimenMinimapComponent::BeginPlay()
{
	Super::BeginPlay();
}

void ULimenMinimapComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

UMaterialInstanceDynamic* ULimenMinimapComponent::GetMiniMapMaterial() const
{
	return RenderTargetMaterialInstance.Get();
}

USceneCaptureComponent2D* ULimenMinimapComponent::GetSceneCapture() const
{
	return SceneCaptureComponent.Get();
}
