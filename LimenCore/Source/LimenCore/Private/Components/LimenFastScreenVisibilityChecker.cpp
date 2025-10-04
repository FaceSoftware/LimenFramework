// Copyright © FaceSoftware. All rights reserved.


#include "Components/LimenFastScreenVisibilityChecker.h"

#include "SceneView.h"
#include "RenderGraphResources.h"
#include "RenderGraphUtils.h"
#include "SceneRenderTargetParameters.h"
#include "Shaders/FFastScreenVisibilityCheckCS.h"


void ULimenFastScreenVisibilityChecker::FFastScreenVisibilityCheckViewExtension::
    PostRenderViewFamily_RenderThread(FRDGBuilder& GraphBuilder, FSceneViewFamily& ViewFamily)
{
	FSceneViewExtensionBase::PostRenderViewFamily_RenderThread(GraphBuilder, ViewFamily);

    if (!Owner.IsValid()) return;
    if (!ViewFamily.EngineShowFlags.Game) return;

    // --- enqueue one pass per view ---
    for (const FSceneView* V : ViewFamily.Views)
    {
        const FSceneView& View = *V;
        const FIntRect ViewRect = View.UnscaledViewRect;

        // 1) 1-uint buffer + UAV (flag)
        FRDGBufferDesc OutFlagBufDesc = FRDGBufferDesc::CreateBufferDesc(sizeof(uint32), 1);
        const FRDGBufferRef  OutFlagBuf = GraphBuilder.CreateBuffer(OutFlagBufDesc, TEXT("Limen.StencilFlag"));
        const FRDGBufferUAVRef OutFlagUAV = GraphBuilder.CreateUAV(OutFlagBuf, PF_R32_UINT);
        AddClearUAVPass(GraphBuilder, OutFlagUAV, 0u);

        // FRDGTextureDesc DebugDesc = FRDGTextureDesc::Create2D(ViewRect.Size(), PF_R8G8B8A8, FClearValueBinding::Black, TexCreate_ShaderResource | TexCreate_UAV);
        // const FRDGTextureRef DebugTex = GraphBuilder.CreateTexture(DebugDesc, TEXT("Limen.DebugStencilOut"));
        // const FRDGTextureUAVRef DebugUAV = GraphBuilder.CreateUAV(FRDGTextureUAVDesc(DebugTex));
        // AddClearUAVPass(GraphBuilder, DebugUAV, FUintVector4(0,0,0,255));

        // 3) Fill params
        auto* Params = GraphBuilder.AllocParameters<FFastScreenVisibilityCheckCS::FParameters>();
        Params->View            = View.ViewUniformBuffer;
        Params->SceneTextures   = CreateSceneTextureUniformBuffer(GraphBuilder, View, ESceneTextureSetupMode::All);
        Params->ViewRectMin     = ViewRect.Min;
        Params->ViewRectSize    = ViewRect.Size();
        Params->Mask            = MaskToCheck;
        Params->OutFlag         = OutFlagUAV;
        // Params->DebugOut        = DebugUAV;

        // 4) Dispatch compute
        TShaderMapRef<FFastScreenVisibilityCheckCS> CS(GetGlobalShaderMap(View.GetFeatureLevel()));
        const FIntPoint Sz = View.UnscaledViewRect.Size();
        const FIntVector Groups( (Sz.X + 7) / 8, (Sz.Y + 7) / 8, 1 );

        FComputeShaderUtils::AddPass(GraphBuilder, RDG_EVENT_NAME("Limen.StencilReduce"), CS, Params, Groups);

        // --- 5) Queue GPU->CPU copy **right here** (buffer path)
        const int WriteIdx = FrameCounter & 1;
        if (!Readbacks[WriteIdx].IsValid())
        {
            Readbacks[WriteIdx] = MakeUnique<FRHIGPUBufferReadback>(TEXT("Limen.StencilFlagRB"));
        }
        // Buffer overload takes a byte size (sizeof(uint32) for one element)
        AddEnqueueCopyPass(GraphBuilder, Readbacks[WriteIdx].Get(), OutFlagBuf, sizeof(uint32));

        // if (UTextureRenderTarget2D* RT = Owner->GetDebugRenderTarget())
        // if (FTextureRenderTargetResource* Res = RT->GetRenderTargetResource())
        // if (FRHITexture* RHI = Res->GetTextureRHI())
        // {
        //     if (FIntPoint(RHI->GetDesc().Extent.X, RHI->GetDesc().Extent.Y ) == ViewRect.Size())
        //     {
        //         FTextureRHIRef ExtRHI = RHI;
        //         const FRDGTextureRef ExtRDG = RegisterExternalTexture(GraphBuilder, ExtRHI, TEXT("Limen.DebugRT"));
        //         AddCopyTexturePass(GraphBuilder, DebugTex, ExtRDG);
        //     }
        // }
    }

    // --- 6) Poll last frame's readback on RT, then bounce to GT
    if (const int ReadIdx = (FrameCounter + 1) & 1; Readbacks[ReadIdx].IsValid() && Readbacks[ReadIdx]->IsReady())
    {
        const void* Ptr = Readbacks[ReadIdx]->Lock(sizeof(uint32));
        const uint32 v = *static_cast<const uint32*>(Ptr);
        Readbacks[ReadIdx]->Unlock();

        // const bool bStencilMaskHit = (v & 0x1) != 0;
        const bool bVisible = (v & 0x2) != 0;

        auto LocalOwner = Owner;
        AsyncTask(ENamedThreads::GameThread, [LocalOwner, bVisible]
        {
            if (LocalOwner.IsValid())
            {
                LocalOwner->VisibilityResultFromRender(bVisible);
            }
        });
    }

    ++FrameCounter;
}

ULimenFastScreenVisibilityChecker::ULimenFastScreenVisibilityChecker(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    bCurrentIsVisible = false;
    StencilMask = 0;
    bAutoActivate = true;
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.TickInterval = 1.0f;
}

void ULimenFastScreenVisibilityChecker::BeginPlay()
{
    Super::BeginPlay();

    // DebugOutputRT = TStrongObjectPtr(NewObject<UTextureRenderTarget2D>(this, TEXT("LimenDebugRT")));
    // DebugOutputRT->RenderTargetFormat = RTF_RGBA8;
    // DebugOutputRT->bAutoGenerateMips = false;
    // DebugOutputRT->ClearColor = FLinearColor::Black;

    FVector2D ViewportSize;
    GEngine->GameViewport->GetViewportSize(ViewportSize);
    // DebugOutputRT->InitAutoFormat(static_cast<int32>(ViewportSize.X), static_cast<int32>(ViewportSize.Y));

    ViewExt = FSceneViewExtensions::NewExtension<FFastScreenVisibilityCheckViewExtension>(TWeakObjectPtr(this),
                                                                                          StencilMask);
}

void ULimenFastScreenVisibilityChecker::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
//  if (DebugOutputRT.IsValid())
//  {
//      DebugOutputRT->ConditionalBeginDestroy();
//      DebugOutputRT.Reset();
//  }
    ViewExt.Reset();
}

void ULimenFastScreenVisibilityChecker::TickComponent(const float DeltaTime, const ELevelTick TickType,
    FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

//  if (DebugOutputRT.IsValid() && GEngine && GEngine->GameViewport)
//  {
//      FVector2D ViewportSize;
//      GEngine->GameViewport->GetViewportSize(ViewportSize);
//      DebugOutputRT->ResizeTarget(static_cast<int32>(ViewportSize.X), static_cast<int32>(ViewportSize.Y));
//  }
}

bool ULimenFastScreenVisibilityChecker::IsVisible() const
{
    return bCurrentIsVisible;
}

UTextureRenderTarget2D* ULimenFastScreenVisibilityChecker::GetDebugRenderTarget() const
{
//  return DebugOutputRT.Get();
    return nullptr;
}

void ULimenFastScreenVisibilityChecker::VisibilityResultFromRender(const bool bIsVisible)
{
    check(IsInGameThread());
    if (bCurrentIsVisible == bIsVisible) return;

    bCurrentIsVisible = bIsVisible;
    OnVisibilityUpdated.Broadcast(bCurrentIsVisible);
}
