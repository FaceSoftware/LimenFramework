// Copyright © FaceSoftware. All rights reserved.


#include "Components/LimenFastScreenVisibilityChecker.h"

#include "SceneView.h"
#include "RenderGraphResources.h"
#include "RenderGraphUtils.h"
#include "SceneRenderTargetParameters.h"
#include "BlueprintLibraries/LimenCoreStatics.h"
#include "LogMacros/LimenLogMacros.h"
#include "Shaders/FFastScreenVisibilityCheckCS.h"


ULimenFastScreenVisibilityChecker::FFastScreenVisibilityCheckViewExtension::FFastScreenVisibilityCheckViewExtension(
    const FAutoRegister& AutoReg, const TWeakObjectPtr<ULimenFastScreenVisibilityChecker> InOwner,
    const TSet<uint32>& InMasks, const int32 InMaximumFrameBuffering) : FSceneViewExtensionBase(AutoReg)
                                                                      , Owner(InOwner)
                                                                      , MasksToCheck(InMasks.Array())
                                                                      , MaximumFrameBuffering(InMaximumFrameBuffering)
                                                                      , bShouldSkip(false)
{
    VisibilityStates.AddZeroed(256);
}

void ULimenFastScreenVisibilityChecker::FFastScreenVisibilityCheckViewExtension::PrePostProcessPass_RenderThread(
    FRDGBuilder& GraphBuilder, const FSceneView& InView, const FPostProcessingInputs& Inputs)
{
    FSceneViewExtensionBase::PrePostProcessPass_RenderThread(GraphBuilder, InView, Inputs);
    
    FScopeLock Lock(&ActiveSection);
    DispatchVisibilityCheck(GraphBuilder, InView);
    ReadbackVisibilityFlags();
}

void ULimenFastScreenVisibilityChecker::FFastScreenVisibilityCheckViewExtension::Deactivate()
{
    FScopeLock Lock(&ActiveSection);
    bShouldSkip = true;
    Readbacks.Empty(256);
}

void ULimenFastScreenVisibilityChecker::FFastScreenVisibilityCheckViewExtension::Activate()
{
    FScopeLock Lock(&ActiveSection);
    bShouldSkip = false;
    VisibilityStates.AddZeroed(256);
}

bool ULimenFastScreenVisibilityChecker::FFastScreenVisibilityCheckViewExtension::IsActiveThisFrame_Internal(
    const FSceneViewExtensionContext& Context) const
{
    FScopeLock Lock(&ActiveSection);
    return !bShouldSkip;
}

void ULimenFastScreenVisibilityChecker::FFastScreenVisibilityCheckViewExtension::DispatchVisibilityCheck(FRDGBuilder& GraphBuilder, const FSceneView& InView)
{
#if WITH_EDITOR
    {
        FScopeLock Lock(&ActiveSection);
        check(!bShouldSkip)
    }
#endif
   
    ViewRect = InView.CameraConstrainedViewRect;
    if (ViewRect.IsEmpty()) { return; }

    if (!Owner.IsValid()) return;
    // if (InView.ViewActor.Actor != Owner->GetOwner()) return;
    if (MaximumFrameBuffering > 0 && Readbacks.Num() >= MaximumFrameBuffering) return;

    // 1) 1-uint buffer + UAV (flag)
    const FRDGBufferDesc OutFlagBufDesc = FRDGBufferDesc::CreateBufferDesc(sizeof(uint32), 256);
    const FRDGBufferRef  OutFlagBuf = GraphBuilder.CreateBuffer(OutFlagBufDesc, TEXT("Limen.StencilFlag"));
    const FRDGBufferUAVRef OutFlagUAV = GraphBuilder.CreateUAV(OutFlagBuf, PF_R32_UINT);
    AddClearUAVPass(GraphBuilder, OutFlagUAV, 0u);

    const FRDGTextureDesc DebugDesc = FRDGTextureDesc::Create2D(ViewRect.Size(),
        PF_R8G8B8A8, FClearValueBinding::Black, TexCreate_ShaderResource | TexCreate_UAV);
    const FRDGTextureRef DebugTex = GraphBuilder.CreateTexture(DebugDesc, TEXT("Limen.DebugStencilOut"));
    const FRDGTextureUAVRef DebugUAV = GraphBuilder.CreateUAV(FRDGTextureUAVDesc(DebugTex));
    AddClearUAVPass(GraphBuilder, DebugUAV, FUintVector4(0,0,0,255));

    const FRDGBufferDesc OutLuminanceBufDesc = FRDGBufferDesc::CreateBufferDesc(sizeof(uint32), 256);
    const FRDGBufferRef  OutLuminanceBuf = GraphBuilder.CreateBuffer(OutLuminanceBufDesc, TEXT("Limen.Luminance"));
    const FRDGBufferUAVRef OutLuminanceUAV = GraphBuilder.CreateUAV(OutLuminanceBuf, PF_R32_UINT);
    AddClearUAVPass(GraphBuilder, OutLuminanceUAV, 0u);
    
    const FRDGBufferRef MasksBuffer = CreateStructuredBuffer(GraphBuilder, TEXT("Visibility.Masks"),
                                                             sizeof(uint32), MasksToCheck.Num(),
                                                             MasksToCheck.GetData(),
                                                             sizeof(uint32) * MasksToCheck.Num());
    
    constexpr ESceneTextureSetupMode SceneTexturesMask = ESceneTextureSetupMode::SceneDepth |
                                                         ESceneTextureSetupMode::CustomDepth;
    
    // 3) Fill params
    using FParameters = FFastScreenVisibilityCheckCS::FParameters;
    FParameters* Params = GraphBuilder.AllocParameters<FParameters>();
    Params->View            = InView.ViewUniformBuffer;
    Params->SceneTextures   = CreateSceneTextureUniformBuffer(GraphBuilder, InView, SceneTexturesMask);
    Params->ViewRectMin     = ViewRect.Min;
    Params->ViewRectSize    = ViewRect.Size();
    Params->Masks           = GraphBuilder.CreateSRV(MasksBuffer);
    Params->OutFlag         = OutFlagUAV;

    // 4) Dispatch compute
    TShaderMapRef<FFastScreenVisibilityCheckCS> ComputeShader(GetGlobalShaderMap(InView.GetFeatureLevel()));
    const FIntPoint Sz = ViewRect.Size();
    const FIntVector Groups( (Sz.X + 7) / 8, (Sz.Y + 7) / 8, 1 );

    FComputeShaderUtils::AddPass(GraphBuilder, RDG_EVENT_NAME("Limen.StencilReduce"), ComputeShader, Params, Groups);

    check(MaximumFrameBuffering <= 0 || Readbacks.Num() < MaximumFrameBuffering)

    Readbacks.Push(MakeShared<FRHIGPUBufferReadback>(TEXT("Limen.StencilFlagRB")));
    AddEnqueueCopyPass(GraphBuilder, Readbacks.Last().Get(), OutFlagBuf, sizeof(uint32) * 256);
}

void ULimenFastScreenVisibilityChecker::FFastScreenVisibilityCheckViewExtension::ReadbackVisibilityFlags()
{
#if WITH_EDITOR
    {
        FScopeLock Lock(&ActiveSection);
        check(!bShouldSkip)
    }
#endif

    for (int32 i = Readbacks.Num() - 1; i >= 0; --i)
    {
        if (!Readbacks[i]->IsReady()) { continue; }
        
        const TSharedPtr<FRHIGPUBufferReadback>& VisibilityReadback = Readbacks[i];
        
        const void* Ptr = VisibilityReadback->Lock(256 * sizeof(uint32));
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
        VisibilityReadback->Unlock();

        Readbacks.RemoveAt(0, i + 1, EAllowShrinking::Yes);
        break;
    }
}

///
///
///

ULimenFastScreenVisibilityChecker::FData::FData(const uint32 Flags)
{
    bIsRendered = (Flags & 1) != 0;
    bIsVisible = (Flags & 2) != 0;
}

ULimenFastScreenVisibilityChecker::ULimenFastScreenVisibilityChecker(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.TickInterval = .2f;
    bAutoActivate = true;
    bEnableDebug = false;
    MaximumFrameBuffering = 1;
}

void ULimenFastScreenVisibilityChecker::BeginPlay()
{
    Super::BeginPlay();

    if (MaximumFrameBuffering <= 0)
    {
        LIMEN_LOG(LogLimenCore, Error, this, TEXT("MaximumFrameBuffering is set to %d! HIGH RISK OF MEMORY LEAKS -> Try a value above 0."), MaximumFrameBuffering)
    }
    
    if (bAutoActivate) { Activate(); }
}

void ULimenFastScreenVisibilityChecker::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
    ViewExt.Reset();
}

void ULimenFastScreenVisibilityChecker::TickComponent(float DeltaTime, enum ELevelTick TickType,
    FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    
    if (bEnableDebug)
    {
        TimeSinceLastUpdate += FTimespan::FromSeconds(DeltaTime);
        const FString Log = FString::Printf(TEXT("Time since last update: %fs"), TimeSinceLastUpdate.GetTotalSeconds());
        LimenLog(this, Log, ELogType::Log, true, TEXT("ULimenFastScreenVisibilityChecker TimeSinceLastUpdate"));
    }
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
            ViewExt = FSceneViewExtensions::NewExtension<FFastScreenVisibilityCheckViewExtension>(this,
                MasksToCheck32, MaximumFrameBuffering);
        }
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
    return VisibilityStates[Mask].IsVisible();
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
    if (!VisibilityStates.Contains(Mask)) { return; }

    if (!IsActive()) return;

    UpdatesThisTick++;
    
    if (bEnableDebug)
    {
        LastUpdate = FDateTime::Now();
        TimeSinceLastUpdate = FTimespan(0);
    }
    
    const FData NewData(VisibilityFlags);
    
    FData& OldDataRef = VisibilityStates[Mask];
    if (NewData == OldDataRef) { return; }

    OldDataRef = NewData;
    OnVisibilityUpdated.Broadcast(Mask, OldDataRef);
}
