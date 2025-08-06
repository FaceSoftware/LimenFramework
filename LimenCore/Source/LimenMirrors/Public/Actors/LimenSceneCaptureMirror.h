// Copyright © 2024 FaceSoftware. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actors/LimenGameplayActor.h"
#include "LimenSceneCaptureMirror.generated.h"


class UMaterialInstanceDynamic;
class UTextureRenderTarget2D;
class UTextureRenderTarget;
class USceneCaptureComponent2D;

UCLASS()
class ALimenSceneCaptureMirror : public ALimenGameplayActor
{
	GENERATED_BODY()

public:
	ALimenSceneCaptureMirror();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<USceneCaptureComponent2D> SceneCapture;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<USceneComponent> SceneCapturePivot;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> MirrorMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName RenderTargetMaterialParameterName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ClipPlaneOffset;

private:
	TWeakObjectPtr<APlayerController> LocalPlayerController;
	FVector2D CachedViewportSize;
	float CachedFOV;
	
};
