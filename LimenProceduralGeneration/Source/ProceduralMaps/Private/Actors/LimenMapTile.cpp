// Copyright Face Software. All Rights Reserved.


#include "Actors/LimenMapTile.h"

#include "EngineUtils.h"
#include "Subsystems/LimenGlobalRandomStreamSubsystem.h"
#include "BlueprintLibraries/LimenMath.h"
#include "Components/BoxComponent.h"
#include "Components/LimenDynamicLoadingComponent.h"
#include "Kismet/GameplayStatics.h"
#include "LogMacros/LimenLogMacros.h"


ALimenMapTile* ALimenMapTile::GetTileOfClass(UObject* WorldContext, const TSubclassOf<ALimenMapTile> TileClass)
{
	check(GEngine)
	const auto* World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::Assert);
	for (const TActorIterator It(World, TileClass); It;)
	{
		auto* Tile = *It;
		return Tile;
	}

	return nullptr;
}

ALimenMapTile::ALimenMapTile()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent->SetMobility(EComponentMobility::Static);

	BoundingBox = CreateDefaultSubobject<UBoxComponent>(TEXT("BoundingBox"));
	check(BoundingBox)
	BoundingBox->SetupAttachment(GetRootComponent());
	BoundingBox->SetMobility(EComponentMobility::Movable);
	BoundingBox->SetCollisionProfileName(TEXT("OverlapAll"));

	DynamicLoadingComponent = CreateDefaultSubobject<ULimenDynamicLoadingComponent>(TEXT("DynamicLoadingComponent"));
	check(DynamicLoadingComponent)
	
	ScenarioComponentClass = USceneComponent::StaticClass();
	ScenarioComponentKeyword = TEXT("Scenario");
	bForceNoScenarios = false;

#if WITH_EDITOR
	PreviewScenarioIndex = -1;
#endif
	
	RandomStream = nullptr;
}

void ALimenMapTile::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	RandomStream = ULimenGlobalRandomStreamSubsystem::Get()->GetGlobalRandomStream().ToSharedPtr();
	check(RandomStream)
	
	Scenarios.Empty();

	if (bForceNoScenarios)
	{
		return;
	}
	
	TArray<UActorComponent*> Components;
	GetComponents(Components, true);
	for (auto* Component : Components)
	{
		auto* Scenario = Cast<USceneComponent>(Component);
		if (IsValid(Scenario) && Component->GetName().Contains(ScenarioComponentKeyword))
		{
			Scenarios.Push(Scenario);
		}
	}

#if WITH_EDITOR
	// Show only preview scenario
	for (int i = 0; i < Scenarios.Num(); ++i)
	{
		Scenarios[i]->SetVisibility(i == PreviewScenarioIndex, true);
	}
#endif

	FVector Origin, Extent;
	GetActorBounds(false, Origin, Extent, true);
	BoundingBox->SetWorldLocation(Origin);
	BoundingBox->SetBoxExtent(Extent);
}

void ALimenMapTile::BeginPlay()
{
	Super::BeginPlay();

	TargetPlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	BoundingBox->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::BoundingBoxBeginOverlap);

	if (!RandomStream.IsValid())
	{
		RandomStream = ULimenGlobalRandomStreamSubsystem::Get()->GetGlobalRandomStream().ToSharedPtr();
		check(RandomStream);
	}
	
	if (!Scenarios.IsEmpty())
	{
		verify(SelectRandomScenario())
	}
	else
	{

		if (bForceNoScenarios)
		{
			Scenarios.Empty();
		}
	}
}

void ALimenMapTile::BeginDestroy()
{
	Super::BeginDestroy();
}

ULimenDynamicLoadingComponent* ALimenMapTile::GetDynamicLoadingComponent() const
{
	return DynamicLoadingComponent.Get();
}

bool ALimenMapTile::GetRelativeActorLocationAndRotation(AActor* InActor, FVector& OutLocation,
	FRotator& OutRotation, FRotator& OutTileRotation) const
{
	if (!IsValid(InActor))
	{
		return false; 
	}
	
	OutLocation = InActor->GetActorLocation() - GetActorLocation();
	OutRotation = InActor->GetActorRotation() - GetActorRotation();
	OutTileRotation = GetActorRotation();
	return true;
}

bool ALimenMapTile::GetRelativePlayerLocationAndRotation(AController* InController, FVector& OutLocation,
	FRotator& OutRotation, FRotator& OutTileRotation) const
{
	if (!IsValid(InController))
	{
		return false; 
	}
	
	OutLocation = InController->GetPawn()->GetActorLocation() - GetActorLocation();
	OutRotation = InController->GetControlRotation().Clamp();
	OutTileRotation = GetActorRotation().Clamp();
	return true;
}

void ALimenMapTile::TeleportActorToRelativeLocationAndRotation(AActor* InActor, const FVector& InRelativeLocation,
                                                               const FRotator& InRelativeRotation, const FRotator& InOtherTileRotation)
{
	const FVector TileRotationVector = InOtherTileRotation.Vector();
	const double Radius = (InActor->GetActorLocation() - GetActorLocation()).Size();

	const double NewX = Radius * FMath::Cos(TileRotationVector.X * PI / 180.0);
	const double NewY = Radius * FMath::Sin(TileRotationVector.Y * PI / 180.0);
	const FVector Location = GetActorLocation() + InRelativeLocation + FVector(NewX, NewY, 0.0);
	const FRotator Rotation = GetActorRotation() + InRelativeRotation - InOtherTileRotation;

	InActor->SetActorLocation(Location, false, nullptr, ETeleportType::TeleportPhysics);
	InActor->SetActorRotation(Rotation, ETeleportType::TeleportPhysics);
	
	LIMEN_LOG(LogLimen, Log, this, "Actor teleported with diff: Location: %s; Rotation: %s", *Location.ToString(), *Rotation.ToString());
}

bool ALimenMapTile::TeleportPawnToRelativeLocationAndRotation(AController* InController,
	const FVector& InRelativeLocation, const FRotator& InRelativeRotation, const FRotator& InOtherTileRotation)
{
	if (!IsValid(InController) || !IsValid(InController->GetPawn()))
	{
		return false;
	}

	const FVector RotationCenter = GetActorLocation();
	FVector NewLocation = GetActorLocation() + InRelativeLocation;
	FRotator NewRotation = (GetActorRotation().Clamp() - InOtherTileRotation.Clamp()).Clamp();
	const FTransform Transform = ULimenMath::RotatePointAroundPoint(NewLocation, RotationCenter, NewRotation.Yaw, FVector::UpVector);

	NewLocation = Transform.GetLocation();
	NewRotation = (Transform.GetRotation().Rotator().Clamp() + InRelativeRotation.Clamp()).Clamp();
	
	InController->GetPawn()->SetActorLocation(NewLocation);
	InController->SetControlRotation(NewRotation);

	return true;
}

ETileType ALimenMapTile::GetTileType() const
{
	return TileType;
}

ANavMeshBoundsVolume* ALimenMapTile::GetNavigationMesh()
{
	return NavigationMesh.Get();
}

FTileInfo ALimenMapTile::GetAssignedTileInfo() const
{
	return AssignedTileInfo;
}

void ALimenMapTile::BoundingBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                            UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	OnBoundingBoxBeginOverlap.Broadcast(OtherActor);
}

void ALimenMapTile::OverrideBoundingBoxCollisionProfileName(const FName& ProfileName)
{
	BoundingBox->SetCollisionProfileName(ProfileName);
}

TArray<USceneComponent*> ALimenMapTile::GetScenarios() const
{
	return Scenarios;
}

void ALimenMapTile::RecursivelyDestroyChildComponents(const USceneComponent* ParentComponent)
{
	TArray<USceneComponent*> ChildComponents;
	ParentComponent->GetChildrenComponents(false, ChildComponents);
	for (auto* Component : ChildComponents)
	{
		RecursivelyDestroyChildComponents(Component);
		Component->DestroyComponent(false);
	}
}

bool ALimenMapTile::SelectRandomScenario()
{
	int SelectedScenarioIndex = RandomStream->RandRange(0, Scenarios.Num() - 1);
	for (int i = 0; i < Scenarios.Num(); ++i)
	{
		if (i == SelectedScenarioIndex)
		{
			Scenarios[i]->SetVisibility(true, true);
		}
		else
		{
			Scenarios[i]->SetVisibility(false, true);
			RecursivelyDestroyChildComponents(Scenarios[i]);
			Scenarios.RemoveAt(i, 1, true);
			i--; // Since we remove an index, make sure we don't step over the next one
			SelectedScenarioIndex--; // This will also change
		}
	}
	
	check(Scenarios.Num() == 1)
	return true;
}

void ALimenMapTile::CheckTileLoadState()
{
	if (TargetPlayerController.IsValid())
	{
		DynamicLoadingComponent->UpdateLoadState();
	}
}
