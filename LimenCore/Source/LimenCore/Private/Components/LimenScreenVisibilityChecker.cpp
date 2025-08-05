// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/LimenScreenVisibilityChecker.h"

#include "TextureResource.h"
#include "Camera/CameraComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"


ULimenScreenVisibilityChecker::ULimenScreenVisibilityChecker()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 1 / 16.f;
	PrimaryComponentTick.bRunOnAnyThread = false;

	RenderTargetSize = FIntPoint(1280, 720);
	StencilCheckerMaterial = FSoftObjectPath(TEXT("/LimenCore/Materials/PostProcess/M_StencilCheck.M_StencilCheck"));
	MaskParameter = TEXT("Mask");
	RenderTargetParameter = TEXT("RenderTarget");
	MaskToCheck = 1;
	bPreviousVisibleState = false;
	bAutoActivate = true;
}

void ULimenScreenVisibilityChecker::BeginPlay()
{
	Super::BeginPlay();

	auto* Inst = UMaterialInstanceDynamic::Create(StencilCheckerMaterial.LoadSynchronous(), this, TEXT("StencilChecker"));
	StencilCheckerInst = TStrongObjectPtr(Inst);
	StencilCheckerInst->SetScalarParameterValue(MaskParameter, MaskToCheck);
	StencilCheckerInst->SetTextureParameterValue(RenderTargetParameter, RenderTarget.Get());
	
	if (const auto* Pawn = GetOwner<APawn>())
	{
		OwnerCamera = Pawn->GetComponentByClass<UCameraComponent>();
	}

	RenderTarget = TStrongObjectPtr(NewObject<UTextureRenderTarget2D>(this));
	RenderTarget->RenderTargetFormat = RTF_RGBA8;
	RenderTarget->ClearColor = FLinearColor::Transparent;
	RenderTarget->InitAutoFormat(RenderTargetSize.X, RenderTargetSize.Y);
	RenderTarget->UpdateResourceImmediate(true);
	RenderTarget->bAutoGenerateMips = true;
	RenderTarget->MipGenSettings = TMGS_FromTextureGroup;

	SceneCapture = TStrongObjectPtr(NewObject<USceneCaptureComponent2D>(this));
	const FAttachmentTransformRules Rules(EAttachmentRule::SnapToTarget, true);
	OwnerCamera.IsValid()
		? SceneCapture->AttachToComponent(OwnerCamera.Get(), Rules)
		: SceneCapture->AttachToComponent(GetOwner()->GetRootComponent(), Rules);
	SceneCapture->SetRelativeTransform(FTransform::Identity);
	SceneCapture->TextureTarget = RenderTarget.Get();
	SceneCapture->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_LegacySceneCapture;
	SceneCapture->bCaptureEveryFrame = false;
	SceneCapture->bAlwaysPersistRenderingState = true;
	SceneCapture->CaptureSource = SCS_FinalColorLDR;
	SceneCapture->PrimaryComponentTick.bCanEverTick = true;
	SceneCapture->PrimaryComponentTick.TickInterval = PrimaryComponentTick.TickInterval;
	SceneCapture->AddOrUpdateBlendable(StencilCheckerInst.Get(), 1.f);
	SceneCapture->FOVAngle = OwnerCamera->FieldOfView;
	SceneCapture->RegisterComponent();
}

void ULimenScreenVisibilityChecker::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	SceneCapture.Reset();
	StencilCheckerInst.Reset();
	RenderTarget.Reset();
	Super::EndPlay(EndPlayReason);
}

void ULimenScreenVisibilityChecker::TickComponent(const float DeltaTime, const ELevelTick TickType,
												  FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!SceneCapture || !StencilCheckerInst.IsValid()) return;
	SceneCapture->CaptureScene();
	StencilCheckerInst->UpdateCachedData();
	FTextureRenderTargetResource* RTResource = SceneCapture->TextureTarget->GameThread_GetRenderTargetResource();

	// Todo: It sucks to do it in the GPU but man, I cannot find a way to offload the work. Use a small render target res I guess...
	TArray<FColor> OutPixels;
	RTResource->ReadPixels(OutPixels);

	const bool bIsVisible = [OutPixels]
	{
		for (const FColor& Pixel : OutPixels)
		{
			if (Pixel != FColor(0.f))
			{
				return true;
			}
		}

		return false;
	}();

	if (bIsVisible != bPreviousVisibleState)
	{
		OnActorVisibilityUpdated.Broadcast(bIsVisible);
		bPreviousVisibleState = bIsVisible;
	}
}
