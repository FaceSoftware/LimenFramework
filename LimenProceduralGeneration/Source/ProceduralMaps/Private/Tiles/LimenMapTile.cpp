// Copyright Face Software. All Rights Reserved.


#include "Tiles/LimenMapTile.h"

#include "EngineUtils.h"
#include "Subsystems/LimenGlobalRandomStreamSubsystem.h"
#include "BlueprintLibraries/LimenMath.h"
#include "Components/BoxComponent.h"
#include "Engine/Engine.h"
#include "GameFramework/Pawn.h"
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

ALimenMapTile::ALimenMapTile(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 1 / 8;
	
	TileCullDistance = 15000.f;
	
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent->SetMobility(EComponentMobility::Static);

	BoundingBox = CreateOptionalDefaultSubobject<UBoxComponent>(TEXT("BoundingBox"));
	if (BoundingBox)
	{
		BoundingBox->SetupAttachment(GetRootComponent());
		BoundingBox->SetMobility(EComponentMobility::Movable);
		BoundingBox->SetCollisionProfileName(TEXT("OverlapAll"));
	}
	
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
	for (UActorComponent* Component : Components)
	{
		if (USceneComponent* Scenario = Cast<USceneComponent>(Component);
			Scenario != nullptr && Component->GetName().Contains(ScenarioComponentKeyword))
		{
			Scenarios.Push(Scenario);
		}

		if (UPrimitiveComponent* Primitive = Cast<UPrimitiveComponent>(Component);
			Primitive != nullptr)
		{
			Primitive->bNeverDistanceCull = false;
			Primitive->SetCullDistance(TileCullDistance);
		}
	}

#if WITH_EDITOR
	// Show only preview scenario
	for (int i = 0; i < Scenarios.Num(); ++i)
	{
		Scenarios[i]->SetVisibility(i == PreviewScenarioIndex, true);
	}
#endif

	if (BoundingBox)
	{
		FVector Origin, Extent;
		GetActorBounds(true, Origin, Extent, true);
		BoundingBox->SetWorldLocation(Origin);
		BoundingBox->SetBoxExtent(Extent);
	}
}

void ALimenMapTile::BeginPlay()
{
	Super::BeginPlay();

	if (!RandomStream.IsValid())
	{
		RandomStream = ULimenGlobalRandomStreamSubsystem::Get()->GetGlobalRandomStream().ToSharedPtr();
		check(RandomStream);
	}
	
	if (!Scenarios.IsEmpty())
	{
		verify(SelectRandomScenario())
	}
	else if (bForceNoScenarios)
	{
		Scenarios.Empty();
	}

	if (BoundingBox)
	{
		BoundingBox->OnComponentBeginOverlap.AddDynamic(this, &ALimenMapTile::BoundingBoxBeginOverlap);
	}
}

void ALimenMapTile::BeginDestroy()
{
	Super::BeginDestroy();
}

void ALimenMapTile::SetMapManager(ALimenProceduralMapManager* MapManager)
{
	BoundMapManager = MapManager;
	MapManagerSet(MapManager);
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
	
	LIMEN_LOG(LogLimen, Log, this, TEXT("Actor teleported with diff: Location: %s; Rotation: %s"), *Location.ToString(), *Rotation.ToString());
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

UTileInfo* ALimenMapTile::GetAssignedTileInfo() const
{
	return AssignedTileInfo.Get();
}

ALimenProceduralMapManager* ALimenMapTile::GetMapManager() const
{
	return BoundMapManager.Get();
}

void ALimenMapTile::MapManagerSet(ALimenProceduralMapManager* MapManager)
{
}

void ALimenMapTile::BoundingBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
											UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	OnBoundingBoxBeginOverlap.Broadcast(OtherActor);
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
			Scenarios.RemoveAt(i, 1, EAllowShrinking::Yes);

			if (i-- < SelectedScenarioIndex)
			{
				SelectedScenarioIndex--;
			}
		}
	}
	
	check(Scenarios.Num() == 1)
	return true;
}
