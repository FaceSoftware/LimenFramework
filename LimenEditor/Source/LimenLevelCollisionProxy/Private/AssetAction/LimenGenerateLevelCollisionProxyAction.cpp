// Copyright FaceSoftware. All rights reserved.


#include "AssetAction/LimenGenerateLevelCollisionProxyAction.h"

#include "EditorUtilityLibrary.h"
#include "FileHelpers.h"
#include "LimenEditor.h"
#include "LevelEditorSubsystem.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "LogMacros/LimenLogMacros.h"
#include "PhysicsEngine/BodySetup.h"
#include "PhysicsEngine/BoxElem.h"


ULimenGenerateLevelCollisionProxyAction::ULimenGenerateLevelCollisionProxyAction()
{
	SupportedClasses.Push(TSoftClassPtr(UWorld::StaticClass()));
}

void ULimenGenerateLevelCollisionProxyAction::GenerateCollisionProxyForSelected()
{
	TArray<FAssetData> SelectedAssets = UEditorUtilityLibrary::GetSelectedAssetData();

	if (SelectedAssets.IsEmpty()) return;

	for (const FAssetData& WorldAsset : SelectedAssets)
	{
		if (!WorldAsset.IsValid())
		{
			LIMEN_LOG(LogLimenEditor, Warning, this, TEXT("Invalid world asset found in selection."));
			continue;
		}

		UWorld* SrcWorld = Cast<UWorld>(WorldAsset.GetAsset());
		if (!SrcWorld) SrcWorld = Cast<UWorld>(StaticLoadObject(UWorld::StaticClass(), nullptr,
											   *WorldAsset.GetSoftObjectPath().ToString()));
		if (!SrcWorld)
		{
			LIMEN_LOG(LogLimenEditor, Warning, this, TEXT("Selected asset is not a UWorld."));
		}

		const FString Base = WorldAsset.PackageName.ToString();
		const FString DestPath = Base + TEXT("_CollisionProxy");
		GenerateProxyForWorld(SrcWorld, DestPath);

	}

}

bool ULimenGenerateLevelCollisionProxyAction::GenerateProxyForWorld(UWorld* SrcWorld, const FString& DestPackagePath)
{
	FScopedSlowTask Task(1.f, FText::FromString(TEXT("Generating collision proxy…"))); Task.MakeDialog(true);

	// Create/open an empty destination map
    const FString DestMapAssetPath = DestPackagePath + TEXT(".") + FPackageName::GetShortName(DestPackagePath);
    UWorld* DestWorld = UEditorLoadingAndSavingUtils::NewBlankMap(true);
    if (!DestWorld) { UE_LOG(LogTemp, Error, TEXT("Failed to load destination world.")); return false; }

	// GEditor->GetEditorSubsystem<ULevelEditorSubsystem>()->LoadLevel(DestPackagePath);
	DestWorld = GEditor->GetEditorWorldContext().World();

	const FString Context = TEXT("LimenLevelCollisionProxy");
	const FText SessionName = FText::FromString(TEXT("Generating collision proxy level"));
    FScopedTransaction Tx(*Context, SessionName, DestWorld, true);

    // Iterate all levels in the source world (handles streamed sublevels as well if they’re loaded)
    TArray<ULevel*> Levels;
    Levels.Add(SrcWorld->PersistentLevel);
    for (ULevel* L : SrcWorld->GetLevels()) if (L && L != SrcWorld->PersistentLevel) Levels.AddUnique(L);

    int32 Count = 0;

    for (ULevel* CurrentLevel : Levels)
    {
        if (!CurrentLevel) continue;

        for (AActor* A : CurrentLevel->Actors)
        {
            if (!A) continue;

            TArray<UStaticMeshComponent*> SMs;
            A->GetComponents<UStaticMeshComponent>(SMs);
            if (SMs.Num() == 0) continue;

            for (UStaticMeshComponent* SMC : SMs)
            {
                if (!SMC || !SMC->GetStaticMesh()) continue;
                const UBodySetup* BS = SMC->GetStaticMesh()->GetBodySetup();
                if (!BS) continue;

                const FTransform C2W = SMC->GetComponentTransform();

                // BOX collision (exact)
                for (const FKBoxElem& B : BS->AggGeom.BoxElems)
                {
                    const FTransform L(B.Rotation.Quaternion(), B.Center);
                    const FTransform W = L * C2W;
                    const FVector Ext(B.X*0.5f, B.Y*0.5f, B.Z*0.5f);
                    SpawnBox(DestWorld, W, Ext);
                    ++Count;
                }

            	// SPHERES (exact)
            	for (const FKSphereElem& E : BS->AggGeom.SphereElems)
            	{
            		const FTransform L(FQuat::Identity, E.Center);
            		const FTransform W = L * C2W;
            		AActor* Proxy = SrcWorld->SpawnActor<AActor>(AActor::StaticClass(), W);
            		USceneComponent* Root = EnsureRoot(Proxy);

            		USphereComponent* S = NewObject<USphereComponent>(Proxy);
            		S->SetupAttachment(Root);
            		S->RegisterComponent();
            		S->SetSphereRadius(E.Radius, false);
            		SetupCommon(S);
            	}

            	// CAPSULES (exact)
            	for (const FKSphylElem& E : BS->AggGeom.SphylElems)
            	{
            		// Capsule length is the straight cylinder section (no hemispheres).
            		const float Radius = E.Radius;
            		const float HalfHeight = E.Length * 0.5f + Radius; // UCapsule half-height includes hemispheres
            		const FTransform L(E.Rotation.Quaternion(), E.Center);
            		const FTransform W = L * C2W;

            		AActor* Proxy = SrcWorld->SpawnActor<AActor>(AActor::StaticClass(), W);
            		USceneComponent* Root = EnsureRoot(Proxy);

            		UCapsuleComponent* C = NewObject<UCapsuleComponent>(Proxy);
            		C->SetupAttachment(Root);
            		C->RegisterComponent();
            		C->SetCapsuleSize(Radius, HalfHeight, false);
            		SetupCommon(C);
            	}

            	// CONVEX (choose: precise or fast)
            	for (const FKConvexElem& E : BS->AggGeom.ConvexElems)
            	{
            		// FAST: oriented box fit (good enough for broad safety collision)
            		FBox LB(ForceInit);
            		for (const FVector& V : E.VertexData) LB += V;
            		if (LB.IsValid)
            		{
            			const FTransform W = FTransform(E.GetTransform()) * C2W;
            			AActor* Proxy = SrcWorld->SpawnActor<AActor>(AActor::StaticClass(), W);
            			USceneComponent* Root = EnsureRoot(Proxy);

            			UBoxComponent* Box = NewObject<UBoxComponent>(Proxy);
            			Box->SetupAttachment(Root);
            			Box->RegisterComponent();
            			Box->SetBoxExtent(LB.GetExtent(), false);
            			SetupCommon(Box);
            		}
            	}
            }
        }
    }

    // Save the generated map
    bool bSaved = UEditorLoadingAndSavingUtils::SaveMap(DestWorld, DestPackagePath);
	
    UE_LOG(LogTemp, Display, TEXT("Generated %d proxy boxes. Saved: %s"), Count, bSaved ? TEXT("Yes") : TEXT("No"));
    return bSaved;
}

USceneComponent* ULimenGenerateLevelCollisionProxyAction::EnsureRoot(AActor* A)
{
	if (A->GetRootComponent())
		return A->GetRootComponent();

	USceneComponent* Root = NewObject<USceneComponent>(A, TEXT("Root"));
	Root->RegisterComponent();
	A->SetRootComponent(Root);
	return Root;
}

AActor* ULimenGenerateLevelCollisionProxyAction::SpawnBox(UWorld* W, const FTransform& Xf, const FVector& Ext)
{
	AActor* A = W->SpawnActor<AActor>(AActor::StaticClass(), Xf);

	USceneComponent* Root = NewObject<USceneComponent>(A, TEXT("Root"));
	Root->RegisterComponent();
	A->SetRootComponent(Root);

	UBoxComponent* Box = NewObject<UBoxComponent>(A);
	Box->SetupAttachment(Root);
	Box->RegisterComponent();
	Box->SetBoxExtent(Ext, false);
	Box->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Box->SetCollisionProfileName(TEXT("BlockAll"));
	Box->SetGenerateOverlapEvents(false);
	Box->SetHiddenInGame(true);

	return A;
}

void ULimenGenerateLevelCollisionProxyAction::SetupCommon(UPrimitiveComponent* P)
{
	if (!P) return;
	P->SetHiddenInGame(true);
	P->SetVisibility(false, true);
	P->SetCastShadow(false);
	P->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	P->SetCollisionProfileName(TEXT("BlockAll"));
	P->SetGenerateOverlapEvents(false);
	P->SetMobility(EComponentMobility::Static);
	// Optional: if you don't want it to affect nav
	P->SetCanEverAffectNavigation(false);
}
