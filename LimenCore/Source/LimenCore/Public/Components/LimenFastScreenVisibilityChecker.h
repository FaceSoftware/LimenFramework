// Copyright © FaceSoftware. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "RHIGPUReadback.h"
#include "SceneViewExtension.h"
#include "Components/ActorComponent.h"
// #include "Engine/TextureRenderTarget2D.h"
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
			const uint32 InMask)
			: FSceneViewExtensionBase(AutoReg)
			, Owner(InOwner)
			, MaskToCheck(InMask)
		{
		}

		virtual void PostRenderViewFamily_RenderThread(FRDGBuilder& GraphBuilder, FSceneViewFamily& ViewFamily) override;

	private:
		TWeakObjectPtr<ULimenFastScreenVisibilityChecker> Owner; // read only on RT
		uint8 MaskToCheck = 1u;

		// double-buffered readbacks to avoid contention
		TUniquePtr<FRHIGPUBufferReadback> Readbacks[2];
		uint64 FrameCounter = 0;
	};

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FVisibilityUpdate, bool, bIsVisible);

	FVisibilityUpdate OnVisibilityUpdated;

	explicit ULimenFastScreenVisibilityChecker(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category="Screen Visibility Checker")
	bool IsVisible() const;
	UFUNCTION(BlueprintCallable, Category="Screen Visibility Checker")
	UTextureRenderTarget2D* GetDebugRenderTarget() const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Screen Visibility Checker")
	uint8 StencilMask;

private:
	TSharedPtr<FSceneViewExtensionBase> ViewExt; 
	bool bCurrentIsVisible;
	// TStrongObjectPtr<UTextureRenderTarget2D> DebugOutputRT;

	void VisibilityResultFromRender(const bool bIsVisible);
};
