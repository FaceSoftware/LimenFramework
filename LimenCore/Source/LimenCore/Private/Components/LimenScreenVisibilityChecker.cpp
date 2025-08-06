// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/LimenScreenVisibilityChecker.h"

#include "RenderGraphUtils.h"
#include "RHIGPUReadback.h"
#include "RHIStaticStates.h"
#include "TextureResource.h"
#include "Camera/CameraComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/Engine.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "Shaders/DownsamplerCS.h"


ULimenScreenVisibilityChecker::ULimenScreenVisibilityChecker()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 1 / 16.f;
	PrimaryComponentTick.bRunOnAnyThread = false;

	RenderTargetSizeScale = .5f;
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

	if (RenderTargetSizeScale <= 0.f)
	{
		RenderTargetSizeScale = 0.1f;
	}

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
	if (const FVector2D RenderTargetSize = GetViewportSize() * RenderTargetSizeScale; RenderTargetSize.X > 0.f && RenderTargetSize.Y > 0.f)
	{
		RenderTarget->InitAutoFormat(RenderTargetSize.X, RenderTargetSize.Y);
	}
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

	if (!SceneCapture || !StencilCheckerInst.IsValid() || !RenderTarget.IsValid()) return;

	const FVector2D NewRenderTargetSize = GetViewportSize() * RenderTargetSizeScale;
	if (NewRenderTargetSize != FVector2D(RenderTarget->SizeX, RenderTarget->SizeY) &&
		NewRenderTargetSize.X > 0.f && NewRenderTargetSize.Y > 0.f)
	{
		RenderTarget->ResizeTarget(NewRenderTargetSize.X, NewRenderTargetSize.Y);
		RenderTarget->UpdateResourceWithParams(UTexture::EUpdateResourceFlags::ForceRebuild);
		RenderTarget->BlockOnAnyAsyncBuild();
	}

	SceneCapture->CaptureScene();
	StencilCheckerInst->UpdateCachedData();

	CheckVisibility(RenderTarget.Get());
	PollReadback();

	// Todo: It sucks to do it in the CPU but man, I cannot find a way to offload the work. Use a small render target res I guess...
	/*
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
	*/
}

int32 ULimenScreenVisibilityChecker::GetStencilMask() const
{
	return MaskToCheck;
}

bool ULimenScreenVisibilityChecker::IsVisible() const
{
	return bPreviousVisibleState;
}

void ULimenScreenVisibilityChecker::CheckVisibility(UTextureRenderTarget2D* InRenderTarget)
{
	ENQUEUE_RENDER_COMMAND(CheckStencilVisibilityGPU)(
		[this, InRenderTarget](FRHICommandListImmediate& RHICmdList)
		{
			FRDGBuilder GraphBuilder(RHICmdList);

			// 1. Register external texture (SceneCapture output)
			FRDGTextureRef MaskTextureRDG = RegisterExternalTexture(GraphBuilder, InRenderTarget->GetRenderTargetResource()->GetRenderTargetTexture(), TEXT("StencilMaskTexture"));

			// 2. Create 1x1 float texture to store result
			FRDGTextureDesc ResultDesc = FRDGTextureDesc::Create2D(FIntPoint(1, 1), PF_R32_FLOAT, FClearValueBinding::Black, TexCreate_UAV | TexCreate_ShaderResource);
			FRDGTextureRef ResultTexture = GraphBuilder.CreateTexture(ResultDesc, TEXT("VisibilityResult"));

			// 3. Dispatch compute shader
			{
				TShaderMapRef<FDownsamplerCS> Shader(GetGlobalShaderMap(GMaxRHIFeatureLevel));
				auto* Params = GraphBuilder.AllocParameters<FDownsamplerCS::FParameters>();
				Params->Input = MaskTextureRDG;
				Params->InputSampler = TStaticSamplerState<SF_Point>::GetRHI();
				Params->Result = GraphBuilder.CreateUAV(ResultTexture);

				FIntPoint Size = { InRenderTarget->SizeX, InRenderTarget->SizeY };
				FComputeShaderUtils::AddPass(
					GraphBuilder,
					RDG_EVENT_NAME("GPUStencilVisibilityReduce"),
					Shader,
					Params,
					FIntVector(FMath::DivideAndRoundUp(Size.X, 8), FMath::DivideAndRoundUp(Size.Y, 8), 1)
				);
			}

			// 4. Read the result pixel
			AddReadbackStep(GraphBuilder, ResultTexture, this);

			GraphBuilder.Execute();
		});
}

void ULimenScreenVisibilityChecker::AddReadbackStep(FRDGBuilder& GraphBuilder, FRDGTextureRef ResultTexture, ULimenScreenVisibilityChecker* Checker)
{
	if (!Checker->Readback.IsValid())
	{
		Checker->Readback = MakeUnique<FRHIGPUTextureReadback>(TEXT("VisibilityCheckerReadback"));
	}
    
	AddEnqueueCopyPass(
		GraphBuilder,
		Checker->Readback.Get(),
		ResultTexture,
		FResolveRect()
	);
}

void ULimenScreenVisibilityChecker::PollReadback()
{
	if (!Readback.IsValid()) return;
	if (!Readback->IsReady()) return;

	int32 Width = 0;
	int32 Height = 0;
	void* DataPtr = Readback->Lock(Width, &Height);
	if (Width == 0 || Height == 0) return;
	if (!DataPtr) return;

	// Validation
	check(Width != 0);
	check(Height != 0);

	// Read your pixel data here, e.g.:
	float* PixelData = static_cast<float*>(DataPtr);
	bool bIsVisible = PixelData[0] > 0.5f;

	if (bIsVisible != bPreviousVisibleState)
	{
		OnActorVisibilityUpdated.Broadcast(bIsVisible);
		bPreviousVisibleState = bIsVisible;
	}

	Readback->Unlock();
}

FVector2D ULimenScreenVisibilityChecker::GetViewportSize()
{
	if (GEngine && GEngine->GameViewport)
	{
		FVector2D ViewportSize;
		GEngine->GameViewport->GetViewportSize(ViewportSize);
		return ViewportSize;
	}

	return FVector2D(0, 0); // fallback if no viewport
}
