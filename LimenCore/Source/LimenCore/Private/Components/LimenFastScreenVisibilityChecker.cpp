// Copyright © FaceSoftware. All rights reserved.


#include "Components/LimenFastScreenVisibilityChecker.h"

#include "SceneView.h"
#include "RenderGraphResources.h"
#include "RenderGraphUtils.h"
#include "SceneRenderTargetParameters.h"
// #include "BlueprintLibraries/LimenCoreStatics.h"
#include "LogMacros/LimenLogMacros.h"
#include "Shaders/FFastScreenVisibilityCheckCS.h"


void ULimenFastScreenVisibilityChecker::FFastScreenVisibilityCheckViewExtension::
PostRenderBasePassDeferred_RenderThread(FRDGBuilder& GraphBuilder, FSceneView& InView,
    const FRenderTargetBindingSlots& RenderTargets, TRDGUniformBufferRef<FSceneTextureUniformParameters> SceneTextures)
{
    FSceneViewExtensionBase::PostRenderBasePassDeferred_RenderThread(GraphBuilder, InView, RenderTargets, SceneTextures);

    if (!Owner.IsValid()) return;
    if (InView.ViewActor.Actor != Owner->GetOwner()) return;
    if (MaximumFrameBuffering <= 0 || Readbacks.Num() < MaximumFrameBuffering) return;

    // 1) 1-uint buffer + UAV (flag)
    FRDGBufferDesc OutFlagBufDesc = FRDGBufferDesc::CreateBufferDesc(sizeof(uint32), 1);
    const FRDGBufferRef  OutFlagBuf = GraphBuilder.CreateBuffer(OutFlagBufDesc, TEXT("Limen.StencilFlag"));
    const FRDGBufferUAVRef OutFlagUAV = GraphBuilder.CreateUAV(OutFlagBuf, PF_R32_UINT);
    AddClearUAVPass(GraphBuilder, OutFlagUAV, 0u);

    ViewRect = InView.CameraConstrainedViewRect;

    FRDGTextureDesc DebugDesc = FRDGTextureDesc::Create2D(ViewRect.Size(),
                                                  PF_R8G8B8A8, FClearValueBinding::Black,
                                                  TexCreate_ShaderResource | TexCreate_UAV);
    const FRDGTextureRef DebugTex = GraphBuilder.CreateTexture(DebugDesc, TEXT("Limen.DebugStencilOut"));
    const FRDGTextureUAVRef DebugUAV = GraphBuilder.CreateUAV(FRDGTextureUAVDesc(DebugTex));
    AddClearUAVPass(GraphBuilder, DebugUAV, FUintVector4(0,0,0,255));

    // 3) Fill params
    auto* Params =
        GraphBuilder.AllocParameters<FFastScreenVisibilityCheckCS::FParameters>();
    Params->View            = InView.ViewUniformBuffer;
    Params->SceneTextures   = CreateSceneTextureUniformBuffer(GraphBuilder, InView,
                                                                 ESceneTextureSetupMode::All);
    Params->ViewRectMin     = ViewRect.Min;
    Params->ViewRectSize    = ViewRect.Size();
    Params->Mask            = MaskToCheck;
    Params->OutFlag         = OutFlagUAV;
    Params->DebugOut        = DebugUAV;

    // 4) Dispatch compute
    TShaderMapRef<FFastScreenVisibilityCheckCS> CS(GetGlobalShaderMap(InView.GetFeatureLevel()));
    const FIntPoint Sz = ViewRect.Size();
    const FIntVector Groups( (Sz.X + 7) / 8, (Sz.Y + 7) / 8, 1 );

    FComputeShaderUtils::AddPass(GraphBuilder, RDG_EVENT_NAME("Limen.StencilReduce"), CS, Params, Groups);

    // --- 5) Queue GPU->CPU copy **right here** (buffer path)
    check(MaximumFrameBuffering <= 0 || Readbacks.Num() < MaximumFrameBuffering)
    Readbacks.Push(MakeShared<FRHIGPUBufferReadback>(TEXT("Limen.StencilFlagRB")));

    // Buffer overload takes a byte size (sizeof(uint32) for one element)
    AddEnqueueCopyPass(GraphBuilder, Readbacks.Last().Get(), OutFlagBuf, sizeof(uint32));

    if (Owner->bEnableDebug)
    if (UTextureRenderTarget2D* RT = Owner->GetDebugRenderTarget())
    if (FTextureRenderTargetResource* Res = RT->GetRenderTargetResource())
    if (FRHITexture* RHI = Res->GetTextureRHI())
    {
        if (FIntPoint(RHI->GetDesc().Extent.X, RHI->GetDesc().Extent.Y ) == ViewRect.Size())
        {
            const FRDGTextureRef ExtRDG = RegisterExternalTexture(GraphBuilder, RHI,
                                                                  TEXT("Limen.DebugRT"));
            AddCopyTexturePass(GraphBuilder, DebugTex, ExtRDG);
        }
    }
}

void ULimenFastScreenVisibilityChecker::FFastScreenVisibilityCheckViewExtension::PostRenderViewFamily_RenderThread(
    FRDGBuilder& GraphBuilder, FSceneViewFamily& ViewFamily)
{
    FSceneViewExtensionBase::PostRenderViewFamily_RenderThread(GraphBuilder, ViewFamily);

    // --- 6) Poll last frame's readback on RT, then bounce to GT
    for (int32 i = Readbacks.Num() - 1; i >= 0; --i)
    {
        if (Readbacks[i].IsValid() && Readbacks[i]->IsReady())
        {
            const TSharedPtr<FRHIGPUBufferReadback> MostUpToDateReadbackReady = Readbacks[i];

            const void* Ptr = MostUpToDateReadbackReady->Lock(sizeof(uint32));
            const uint32 v = *static_cast<const uint32*>(Ptr);
            MostUpToDateReadbackReady->Unlock();

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

            Readbacks.RemoveAt(0, i + 1, EAllowShrinking::Yes);
            break;
        }
    }
}

ULimenFastScreenVisibilityChecker::ULimenFastScreenVisibilityChecker(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.TickInterval = 1.f;
    bAutoActivate = true;

    bCurrentIsVisible = false;
    StencilMask = 0;
    bEnableDebug = false;
    MaximumFrameBuffering = 1;
}

void ULimenFastScreenVisibilityChecker::BeginPlay()
{
    Super::BeginPlay();

    if (bEnableDebug)
    {
        DebugOutputRT = TStrongObjectPtr(NewObject<UTextureRenderTarget2D>(this, TEXT("LimenDebugRT")));
        DebugOutputRT->RenderTargetFormat = RTF_RGBA8;
        DebugOutputRT->bAutoGenerateMips = false;
        DebugOutputRT->ClearColor = FLinearColor::Black;
        DebugOutputRT->InitCustomFormat(1, 1, EPixelFormat::PF_R8G8B8A8, false);
    }

    if (MaximumFrameBuffering <= 0)
    {
        LIMEN_LOG(LogLimenCore, Error, this, TEXT("MaximumFrameBuffering is set to %d! HIGH RISK OF MEMORY LEAKS -> Try a value above 0."), MaximumFrameBuffering)
    }

    ViewExt = FSceneViewExtensions::NewExtension<FFastScreenVisibilityCheckViewExtension>(
        TWeakObjectPtr(this), StencilMask, MaximumFrameBuffering);
}

void ULimenFastScreenVisibilityChecker::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
    if (DebugOutputRT.IsValid())
    {
        DebugOutputRT->ConditionalBeginDestroy();
        DebugOutputRT.Reset();
    }
    ViewExt.Reset();
}

void ULimenFastScreenVisibilityChecker::TickComponent(float DeltaTime, enum ELevelTick TickType,
    FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (DebugOutputRT.IsValid() && ViewExt.IsValid())
    {
        const FIntPoint ViewRectSize = ViewExt->GetViewRect().Size();
        DebugOutputRT->ResizeTarget(FMath::Max(ViewRectSize.X, 1), FMath::Max(ViewRectSize.Y, 1));
    }

    // LimenLog(this, FString::Printf(TEXT("Is visible: %d"), bCurrentIsVisible),
    //          ELogType::Log,
    //          true,
    //          TEXT("ULimenFastScreenVisibilityChecker"));
    // 
    // LimenLog(this, FString::Printf(TEXT("Updates: %llu"), UpdatesThisTick),
    //          ELogType::Log,
    //          true,
    //          TEXT("ULimenFastScreenVisibilityChecker UpdatesThisTick"));

    UpdatesThisTick = 0;
}

bool ULimenFastScreenVisibilityChecker::IsVisible() const
{
    return bCurrentIsVisible;
}

UTextureRenderTarget2D* ULimenFastScreenVisibilityChecker::GetDebugRenderTarget() const
{
    return DebugOutputRT.Get();
}

void ULimenFastScreenVisibilityChecker::VisibilityResultFromRender(const bool bIsVisible)
{
    check(IsInGameThread());

    UpdatesThisTick++;
    if (bCurrentIsVisible == bIsVisible) return;

    bCurrentIsVisible = bIsVisible;
    OnVisibilityUpdated.Broadcast(bCurrentIsVisible);
}
