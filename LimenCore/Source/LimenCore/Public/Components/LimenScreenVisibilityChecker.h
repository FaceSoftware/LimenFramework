// Copyright FaceSoftware. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "RenderGraphBuilder.h"
#include "Components/ActorComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "LimenScreenVisibilityChecker.generated.h"


class FRHIGPUTextureReadback;
class UTextureRenderTarget2D;
class USceneCaptureComponent2D;
class UCameraComponent;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FVisibilityUpdate, bool, bIsVisible);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LIMENCORE_API ULimenScreenVisibilityChecker : public UActorComponent
{
	GENERATED_BODY()

public:
	FVisibilityUpdate OnActorVisibilityUpdated;

	ULimenScreenVisibilityChecker();
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
							   FActorComponentTickFunction* ThisTickFunction) override;

	int32 GetStencilMask() const;
	bool IsVisible() const;

protected:
#if WITH_EDITORONLY_DATA
	UPROPERTY(EditDefaultsOnly, Category="Render")
	TSoftObjectPtr<UTextureRenderTarget2D> DebugVisualizationRenderTarget;
#endif

	UPROPERTY(EditDefaultsOnly, Category="Render")
	float RenderTargetSizeScale;
	UPROPERTY(EditDefaultsOnly, Category="Material")
	TSoftObjectPtr<UMaterialInterface> StencilCheckerMaterial;
	UPROPERTY(EditDefaultsOnly, Category="Material")
	FName MaskParameter;
	UPROPERTY(EditDefaultsOnly, Category="Material")
	int32 MaskToCheck;

private:
	TStrongObjectPtr<USceneCaptureComponent2D> SceneCapture;
	TStrongObjectPtr<UMaterialInstanceDynamic> StencilCheckerInst;
	TStrongObjectPtr<UTextureRenderTarget2D> RenderTarget;
	TWeakObjectPtr<UCameraComponent> OwnerCamera;
	bool bPreviousVisibleState;
	
	TUniquePtr<FRHIGPUTextureReadback> Readback;

	void CheckVisibility(UTextureRenderTarget2D* InRenderTarget);
	void AddReadbackStep(FRDGBuilder& GraphBuilder, FRDGTextureRef ResultTexture, ULimenScreenVisibilityChecker* Checker);
	void PollReadback();

	FIntPoint GetRenderTargetSize() const;
};
