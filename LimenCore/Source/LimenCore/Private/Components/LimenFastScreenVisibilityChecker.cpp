// Copyright © FaceSoftware. All rights reserved.


#include "Components/LimenFastScreenVisibilityChecker.h"

#include "SceneView.h"
#include "RenderGraphResources.h"
#include "RenderGraphUtils.h"
#include "SceneRenderTargetParameters.h"
// #include "BlueprintLibraries/LimenCoreStatics.h"
#include "LogMacros/LimenLogMacros.h"
#include "Shaders/FFastScreenVisibilityCheckCS.h"


ULimenFastScreenVisibilityChecker::FFastScreenVisibilityCheckViewExtension::FFastScreenVisibilityCheckViewExtension(
    const FAutoRegister& AutoReg, const TWeakObjectPtr<ULimenFastScreenVisibilityChecker> InOwner,
    const TSet<uint32>& InMasks, const int32 InMaximumFrameBuffering): FSceneViewExtensionBase(AutoReg)
                                                                             , Owner(InOwner)
                                                                             , MasksToCheck(InMasks.Array())
                                                                             , MaximumFrameBuffering(InMaximumFrameBuffering)
                                                                             , ShouldSkip(false)
{
    VisibilityStates.AddZeroed(256);
}

void ULimenFastScreenVisibilityChecker::FFastScreenVisibilityCheckViewExtension::
PostRenderBasePassDeferred_RenderThread(FRDGBuilder& GraphBuilder, FSceneView& InView,
    const FRenderTargetBindingSlots& RenderTargets, TRDGUniformBufferRef<FSceneTextureUniformParameters> SceneTextures)
{
    FSceneViewExtensionBase::PostRenderBasePassDeferred_RenderThread(GraphBuilder, InView, RenderTargets, SceneTextures);
    
    if (ShouldSkip.load(std::memory_order::acquire)) { return; }
   
    ViewRect = InView.CameraConstrainedViewRect;
    if (ViewRect.IsEmpty()) { return; }

    if (!Owner.IsValid()) return;
    // if (InView.ViewActor.Actor != Owner->GetOwner()) return;
    if (MaximumFrameBuffering > 0 && Readbacks.Num() >= MaximumFrameBuffering) return;

    // 1) 1-uint buffer + UAV (flag)
    FRDGBufferDesc OutFlagBufDesc = FRDGBufferDesc::CreateBufferDesc(sizeof(uint32), 256);
    const FRDGBufferRef  OutFlagBuf = GraphBuilder.CreateBuffer(OutFlagBufDesc, TEXT("Limen.StencilFlag"));
    const FRDGBufferUAVRef OutFlagUAV = GraphBuilder.CreateUAV(OutFlagBuf, PF_R32_UINT);
    AddClearUAVPass(GraphBuilder, OutFlagUAV, 0u);

    FRDGTextureDesc DebugDesc = FRDGTextureDesc::Create2D(ViewRect.Size(),
        PF_R8G8B8A8, FClearValueBinding::Black, TexCreate_ShaderResource | TexCreate_UAV);
    const FRDGTextureRef DebugTex = GraphBuilder.CreateTexture(DebugDesc, TEXT("Limen.DebugStencilOut"));
    const FRDGTextureUAVRef DebugUAV = GraphBuilder.CreateUAV(FRDGTextureUAVDesc(DebugTex));
    AddClearUAVPass(GraphBuilder, DebugUAV, FUintVector4(0,0,0,255));

    // 3) Fill params
    auto* Params =
        GraphBuilder.AllocParameters<FFastScreenVisibilityCheckCS::FParameters>();
    Params->View            = InView.ViewUniformBuffer;
    Params->SceneTextures   = CreateSceneTextureUniformBuffer(GraphBuilder, InView,
                                                                 ESceneTextureSetupMode::SceneDepth |
                                                                 ESceneTextureSetupMode::CustomDepth |
                                                                 ESceneTextureSetupMode::SceneColor);
    Params->ViewRectMin     = ViewRect.Min;
    Params->ViewRectSize    = ViewRect.Size();
    {
        const FRDGBufferRef MasksBuffer = CreateStructuredBuffer(GraphBuilder, TEXT("Visibility.Masks"),
            sizeof(uint32), MasksToCheck.Num(), MasksToCheck.GetData(),
            sizeof(uint32) * MasksToCheck.Num());
        Params->Masks = GraphBuilder.CreateSRV(MasksBuffer);
    }
    Params->OutFlag = OutFlagUAV;
    Params->DebugOut = DebugUAV;
    Params->LuminanceThreshold = TakeLuminanceIntoAccount.load(std::memory_order_relaxed) ? LuminanceThreshold.load(std::memory_order_relaxed) : 0.0f;

    // 4) Dispatch compute
    TShaderMapRef<FFastScreenVisibilityCheckCS> CS(GetGlobalShaderMap(InView.GetFeatureLevel()));
    const FIntPoint Sz = ViewRect.Size();
    const FIntVector Groups( (Sz.X + 7) / 8, (Sz.Y + 7) / 8, 1 );

    FComputeShaderUtils::AddPass(GraphBuilder, RDG_EVENT_NAME("Limen.StencilReduce"), CS, Params, Groups);

    // --- 5) Queue GPU->CPU copy **right here** (buffer path)
    check(MaximumFrameBuffering <= 0 || Readbacks.Num() < MaximumFrameBuffering)
    Readbacks.Push(MakeShared<FRHIGPUBufferReadback>(TEXT("Limen.StencilFlagRB")));

    // Buffer overload takes a byte size (sizeof(uint32) for one element)
    AddEnqueueCopyPass(GraphBuilder, Readbacks.Last().Get(), OutFlagBuf, sizeof(uint32) * 256);

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
    
    if (ShouldSkip) { return; }

    for (int32 i = Readbacks.Num() - 1; i >= 0; --i)
    {
        if (!Readbacks[i].IsValid() || !Readbacks[i]->IsReady()) { continue; }
        
        const TSharedPtr<FRHIGPUBufferReadback> Readback = Readbacks[i];

        const void* Ptr = Readback->Lock(256 * sizeof(uint32));
        const uint32* Flags = static_cast<const uint32*>(Ptr);
        for (const uint8& Mask : MasksToCheck)
        {
            const uint32& VisibilityFlags = Flags[Mask];
            
            TWeakObjectPtr WeakOwner = Owner;
            AsyncTask(ENamedThreads::GameThread, [WeakOwner, Mask, VisibilityFlags]
            {
                if (WeakOwner.IsValid())
                {
                    WeakOwner->VisibilityResultFromRender(Mask, VisibilityFlags);
                }
            });
        }
        Readback->Unlock();

        Readbacks.RemoveAt(0, i + 1, EAllowShrinking::Yes);
        break;
    }
}

void ULimenFastScreenVisibilityChecker::FFastScreenVisibilityCheckViewExtension::Deactivate()
{
    ShouldSkip.store(true, std::memory_order::release);
    Readbacks.Empty();
}

void ULimenFastScreenVisibilityChecker::FFastScreenVisibilityCheckViewExtension::Activate()
{
    ShouldSkip.store(true, std::memory_order::release);
    ShouldSkip = false;
    VisibilityStates.AddZeroed(256);
}

///
///
///

ULimenFastScreenVisibilityChecker::FData::FData(const uint32 Flags)
{
    bIsRendered = (Flags & 1) != 0;
    bIsOccluded = (Flags & 2) != 0;
    bIsDark = (Flags & 4) != 0;
}

ULimenFastScreenVisibilityChecker::ULimenFastScreenVisibilityChecker(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.TickInterval = 1.f;
    bAutoActivate = true;
    bEnableDebug = false;
    MaximumFrameBuffering = 1;
    bTakeLuminanceIntoAccount = false;
    LuminanceThreshold = 0.03f;
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
    
    if (bAutoActivate) { Activate(); }
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

void ULimenFastScreenVisibilityChecker::Activate(bool bReset)
{
    // Super::Activate(bReset);

    if (ShouldActivate())
    {
        VisibilityStates.Empty(StencilMasks.Num());
        for (const uint8& Mask : StencilMasks)
        {
            VisibilityStates.Add(Mask);
        }
        
        if (!ViewExt.IsValid())
        {
            TSet<uint32> MasksToCheck32;
            for (const uint8& Mask : StencilMasks)
            {
                MasksToCheck32.Add(Mask);
            }

            // Pass MasksToCheck32 to NewExtension
            ViewExt = FSceneViewExtensions::NewExtension<FFastScreenVisibilityCheckViewExtension>(
                this, MasksToCheck32, MaximumFrameBuffering);
        }
        ViewExt->TakeLuminanceIntoAccount.store(bTakeLuminanceIntoAccount, std::memory_order::relaxed);
        ViewExt->LuminanceThreshold.store(LuminanceThreshold, std::memory_order::relaxed);
        ViewExt->Activate();
        
        
        SetComponentTickEnabled(true);
        SetActiveFlag(true);

        OnComponentActivated.Broadcast(this, bReset);
    }
}

void ULimenFastScreenVisibilityChecker::Deactivate()
{
    // Super::Deactivate();
    if (ViewExt.IsValid() && !ShouldActivate())
    {
        ViewExt->Deactivate();
       
        SetComponentTickEnabled(false);
        SetActiveFlag(false);

        OnComponentDeactivated.Broadcast(this);
    }

}

bool ULimenFastScreenVisibilityChecker::ShouldActivate() const
{
    return Super::ShouldActivate() && HasBegunPlay();
}

bool ULimenFastScreenVisibilityChecker::IsVisible(const uint8 Mask) const
{
    return VisibilityStates[Mask].bIsRendered && !VisibilityStates[Mask].bIsOccluded && !VisibilityStates[Mask].bIsDark;
}

UTextureRenderTarget2D* ULimenFastScreenVisibilityChecker::GetDebugRenderTarget() const
{
    return DebugOutputRT.Get();
}

void ULimenFastScreenVisibilityChecker::AddStencilMask(const uint8 InMask)
{
    StencilMasks.Add(InMask);
    VisibilityStates.Add(InMask, FData(0u));
    if (ViewExt.IsValid()) { ViewExt->MasksToCheck.Add(InMask); }
}

void ULimenFastScreenVisibilityChecker::RemoveStencilMask(const uint8 InMask)
{
    StencilMasks.Remove(InMask);
    VisibilityStates.Remove(InMask);
    if (ViewExt.IsValid()) { ViewExt->MasksToCheck.Remove(InMask); }
}

void ULimenFastScreenVisibilityChecker::VisibilityResultFromRender(const uint8 Mask, const uint32 VisibilityFlags)
{
    check(IsInGameThread())
    check(VisibilityStates.Contains(Mask))

    if (!IsActive()) return;

    UpdatesThisTick++;
    
    const FData NewData(VisibilityFlags);
    
    FData& OldDataRef = VisibilityStates[Mask];
    if (NewData == OldDataRef) { return; }

    OldDataRef = NewData;
    OnVisibilityUpdated.Broadcast(Mask, OldDataRef);
}
