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
			const TSet<uint32>& InMasks,
			const int32 InMaximumFrameBuffering);

		virtual void PostRenderBasePassDeferred_RenderThread(FRDGBuilder& GraphBuilder, FSceneView& InView, const FRenderTargetBindingSlots& RenderTargets, TRDGUniformBufferRef<FSceneTextureUniformParameters> SceneTextures) override;
		virtual void PostRenderViewFamily_RenderThread(FRDGBuilder& GraphBuilder, FSceneViewFamily& ViewFamily) override;

		FORCEINLINE FIntRect GetViewRect() const { return ViewRect; }
		
		void Deactivate();
		void Activate();

	private:
		TWeakObjectPtr<ULimenFastScreenVisibilityChecker> Owner; // read-only on RT
		TArray<uint32> MasksToCheck;

		TArray<TSharedPtr<FRHIGPUBufferReadback>> Readbacks;
		int32 MaximumFrameBuffering;
		FIntRect ViewRect;
		TArray<uint32> VisibilityStates;
		std::atomic<bool> ShouldSkip;
		std::atomic<bool> TakeLuminanceIntoAccount;
		std::atomic<float> LuminanceThreshold;
	};

public:
	struct FData
	{
		FData() = default;
		explicit FData(uint32 Flags);

		bool bIsRendered;
		bool bIsOccluded;
		bool bIsDark;
		
		bool IsVisible() const { return bIsRendered && !bIsOccluded && !bIsDark; }
		bool operator==(const FData& Other) const
		{
			return bIsRendered == Other.bIsRendered && bIsOccluded == Other.bIsOccluded && bIsDark == Other.bIsDark;
		}
	};
	
	DECLARE_MULTICAST_DELEGATE_TwoParams(FVisibilityUpdate, uint8 /* Mask */, const FData& /* VisibilityData */);
	FVisibilityUpdate OnVisibilityUpdated;

	explicit ULimenFastScreenVisibilityChecker(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
							   FActorComponentTickFunction* ThisTickFunction) override;
	virtual void Activate(bool bReset = false) override;
	virtual void Deactivate() override;
	virtual bool ShouldActivate() const override;

	UFUNCTION(BlueprintCallable, Category="Screen Visibility Checker")
	bool IsVisible(uint8 Mask) const;
	UFUNCTION(BlueprintCallable, Category="Screen Visibility Checker")
	UTextureRenderTarget2D* GetDebugRenderTarget() const;
	UFUNCTION(BlueprintCallable, Category="Screen Visibility Checker")
	void AddStencilMask(uint8 InMask);
	UFUNCTION(BlueprintCallable, Category="Screen Visibility Checker")
	void RemoveStencilMask(uint8 InMask);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Screen Visibility Checker")
	TSet<uint8> StencilMasks;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Screen Visibility Checker")
	bool bEnableDebug;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Screen Visibility Checker")
	int32 MaximumFrameBuffering;
	/**
	 * @brief Recommended to disable auto exposure if this is enabled.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Screen Visibility Checker")
	bool bTakeLuminanceIntoAccount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Screen Visibility Checker", meta=(EditCondition="bTakeLuminanceIntoAccount", ClampMin="0", ClampMax="1"))
	float LuminanceThreshold;

private:
	TSharedPtr<FFastScreenVisibilityCheckViewExtension> ViewExt; 
	TMap<uint32, FData> VisibilityStates;
	TStrongObjectPtr<UTextureRenderTarget2D> DebugOutputRT;
	uint64 UpdatesThisTick;

	void VisibilityResultFromRender(uint8 Mask, uint32 VisibilityFlags);
};
