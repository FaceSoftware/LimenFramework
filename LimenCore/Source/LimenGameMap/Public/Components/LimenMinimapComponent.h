// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/SceneCaptureComponent2D.h"
#include "LimenMinimapComponent.generated.h"


class USpringArmComponent;
class UTextureRenderTarget2D;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LIMENGAMEMAP_API ULimenMinimapComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	ULimenMinimapComponent();
	virtual void InitializeComponent() override;
	virtual void UninitializeComponent() override;
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Minimap")
	UMaterialInstanceDynamic* GetMiniMapMaterial() const;
	UFUNCTION(BlueprintCallable, Category = "Minimap")
	USceneCaptureComponent2D* GetSceneCapture() const;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Minimap")
	TObjectPtr<UTextureRenderTarget2D> MiniMapRenderTarget;
	UPROPERTY(EditDefaultsOnly, Category = "Minimap")
	TSoftObjectPtr<UMaterialInterface> RenderTargetMaterial;
	UPROPERTY(EditDefaultsOnly, Category = "Minimap")
	FName MaterialParameter;
	UPROPERTY(EditDefaultsOnly, Category = "Minimap")
	FVector MiniMapRootOffset;

private:
	TStrongObjectPtr<UMaterialInstanceDynamic> RenderTargetMaterialInstance;
	TStrongObjectPtr<USceneCaptureComponent2D> SceneCaptureComponent;
	float OwnerHalfHeight;
};
