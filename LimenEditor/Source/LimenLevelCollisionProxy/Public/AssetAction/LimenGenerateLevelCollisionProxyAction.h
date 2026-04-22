// Copyright FaceSoftware. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Editor/Blutility/Classes/AssetActionUtility.h"
#include "LimenGenerateLevelCollisionProxyAction.generated.h"

/**
 * 
 */
UCLASS()
class LIMENLEVELCOLLISIONPROXY_API ULimenGenerateLevelCollisionProxyAction : public UAssetActionUtility
{
	GENERATED_BODY()

public:
	ULimenGenerateLevelCollisionProxyAction();

	UFUNCTION(CallInEditor, Category="Collision Proxy")
	void GenerateCollisionProxyForSelected();

protected:
	FORCEINLINE static USceneComponent* EnsureRoot(AActor* A);
	FORCEINLINE static AActor* SpawnBox(UWorld* W, const FTransform& Xf, const FVector& Ext);
	FORCEINLINE static void SetupCommon(UPrimitiveComponent* P);

	virtual bool GenerateProxyForWorld(UWorld* SrcWorld, const FString& DestPackagePath);

private:
};
