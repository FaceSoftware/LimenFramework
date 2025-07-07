// Copyright © 2024 FaceSoftware. All rights reserved.


#include "Actors/LimenSceneCaptureMirror.h"

#include "Components/SceneCaptureComponent2D.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/Engine.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "Materials/MaterialInstanceDynamic.h"


ALimenSceneCaptureMirror::ALimenSceneCaptureMirror()
{
	RenderTargetMaterialParameterName = TEXT("Texture");
	ClipPlaneOffset = -3.f;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	MirrorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MirrorMesh"));
	MirrorMesh->SetupAttachment(GetRootComponent());

	SceneCapturePivot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneCapturePivot"));
	SceneCapturePivot->SetupAttachment(MirrorMesh.Get());

	SceneCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SceneCapture"));
	SceneCapture->SetupAttachment(SceneCapturePivot.Get());
	SceneCapture->bEnableClipPlane = true;
}

void ALimenSceneCaptureMirror::BeginPlay()
{
	Super::BeginPlay();

	if (auto* Inst = Cast<UMaterialInstanceDynamic>(MirrorMesh->GetMaterial(0)))
	{
		Inst->SetTextureParameterValue(RenderTargetMaterialParameterName, SceneCapture->TextureTarget.Get());
	}
}

void ALimenSceneCaptureMirror::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector2D ViewportSize;
	GEngine->GameViewport->GetViewportSize(ViewportSize);
	if (SceneCapture->TextureTarget && ViewportSize != CachedViewportSize)
	{
		CachedViewportSize = ViewportSize;
		SceneCapture->TextureTarget->SizeX = FMath::RoundToInt(CachedViewportSize.X);
		SceneCapture->TextureTarget->SizeY = FMath::RoundToInt(CachedViewportSize.Y);
	}

	if (SceneCapture->bEnableClipPlane)
	{
		SceneCapture->ClipPlaneBase = SceneCapturePivot->GetComponentLocation() + SceneCapturePivot->GetForwardVector() * ClipPlaneOffset;
		SceneCapture->ClipPlaneNormal = SceneCapturePivot->GetForwardVector();
	}


	if (!LocalPlayerController.IsValid())
	{
		LocalPlayerController = GetWorld()->GetFirstPlayerController();
	}
	else
	{
		
		if (LocalPlayerController->PlayerCameraManager)
		{
			if (const float CurrentFOV = LocalPlayerController->PlayerCameraManager->GetFOVAngle();
				CurrentFOV != CachedFOV)
			{
				CachedFOV = CurrentFOV;
				SceneCapture->FOVAngle = CachedFOV;
			}
		}

		const FVector EyesLocation = LocalPlayerController->PlayerCameraManager->GetCameraLocation();
		const FRotator EyesRotation = LocalPlayerController->PlayerCameraManager->GetCameraRotation();

		FTransform MirrorRelevantTransform = MirrorMesh->GetComponentTransform();
		MirrorRelevantTransform.SetScale3D(FVector::OneVector);

		const FVector PlayerLocationRelativeToMirror = MirrorRelevantTransform.InverseTransformPosition(EyesLocation);
		FVector SceneCaptureLocation = PlayerLocationRelativeToMirror;
		SceneCaptureLocation.X *= -1;

		const FRotator PlayerRelativeRotationToMirror = MirrorRelevantTransform.InverseTransformRotation(
			EyesRotation.Quaternion()).Rotator();

		FRotator SceneCaptureRotation = PlayerRelativeRotationToMirror.Clamp();
		SceneCaptureRotation.Yaw += 180.f;
		SceneCaptureRotation.Yaw *= -1.f;
		SceneCaptureRotation.Roll *= -1.f;

		SceneCapture->SetRelativeLocationAndRotation(SceneCaptureLocation, SceneCaptureRotation);
	}

	if (!SceneCapture->bCaptureEveryFrame)
	{
		SceneCapture->CaptureScene();
	}
}
