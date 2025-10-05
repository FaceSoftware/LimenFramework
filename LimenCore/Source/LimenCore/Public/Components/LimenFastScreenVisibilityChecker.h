// Copyright © FaceSoftware. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "RHIGPUReadback.h"
#include "SceneViewExtension.h"
#include "Components/ActorComponent.h"
#include "Engine/TextureRenderTarget2D.h"
#include "LimenFastScreenVisibilityChecker.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LIMENCORE_API ULimenFastScreenVisibilityChecker : public UActorComponent
{
	GENERATED_BODY()

	class FFastScreenVisibilityCheckViewExtension : public FSceneViewExtensionBase
	{
		friend ULimenFastScreenVisibilityChecker;

	public:
		FFastScreenVisibilityCheckViewExtension(
			const FAutoRegister& AutoReg,
			const TWeakObjectPtr<ULimenFastScreenVisibilityChecker> InOwner,
			const uint32 InMask,
			int32 InMaximumFrameBuffering)
			: FSceneViewExtensionBase(AutoReg)
			, Owner(InOwner)
			, MaskToCheck(InMask)
			, MaximumFrameBuffering(InMaximumFrameBuffering)
		{
		}

		virtual void PostRenderBasePassDeferred_RenderThread(FRDGBuilder& GraphBuilder, FSceneView& InView, const FRenderTargetBindingSlots& RenderTargets, TRDGUniformBufferRef<FSceneTextureUniformParameters> SceneTextures) override;
		virtual void PostRenderViewFamily_RenderThread(FRDGBuilder& GraphBuilder, FSceneViewFamily& ViewFamily) override;

		FORCEINLINE FIntRect GetViewRect() const { return ViewRect; }

	private:
		TWeakObjectPtr<ULimenFastScreenVisibilityChecker> Owner; // read only on RT
		uint8 MaskToCheck = 1u;

		TArray<TSharedPtr<FRHIGPUBufferReadback>> Readbacks;
		int32 MaximumFrameBuffering;
		FIntRect ViewRect;
	};

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FVisibilityUpdate, bool, bIsVisible);

	FVisibilityUpdate OnVisibilityUpdated;

	explicit ULimenFastScreenVisibilityChecker(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
							   FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category="Screen Visibility Checker")
	bool IsVisible() const;
	UFUNCTION(BlueprintCallable, Category="Screen Visibility Checker")
	UTextureRenderTarget2D* GetDebugRenderTarget() const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Screen Visibility Checker")
	uint8 StencilMask;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Screen Visibility Checker")
	bool bEnableDebug;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Screen Visibility Checker")
	int32 MaximumFrameBuffering;

private:
	TSharedPtr<FFastScreenVisibilityCheckViewExtension> ViewExt; 
	bool bCurrentIsVisible;
	TStrongObjectPtr<UTextureRenderTarget2D> DebugOutputRT;
	uint64 UpdatesThisTick;

	void VisibilityResultFromRender(const bool bIsVisible);
};
