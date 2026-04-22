// Copyright FaceSoftware. All rights reserved.


#include "Components/LimenGridSpawner.h"

#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"


ULimenGridSpawner::ULimenGridSpawner()
{
	PrimaryComponentTick.bCanEverTick = false;

	GridX = 3;
	GridY = 3;
	GridNodeOffset = FVector2D(10.f);
}

void ULimenGridSpawner::OnComponentCreated()
{
	Super::OnComponentCreated();
}

void ULimenGridSpawner::OnComponentDestroyed(const bool bDestroyingHierarchy)
{
	Super::OnComponentDestroyed(bDestroyingHierarchy);
}

void ULimenGridSpawner::BeginPlay()
{
	Super::BeginPlay();

	CreateGrid();
}

bool ULimenGridSpawner::CanCreateGrid() const
{
	return GridNodeActorClass && GridX >= 3 && GridY >= 3 && GetWorld() && !HasAnyFlags(RF_ClassDefaultObject);
}

TArray<UChildActorComponent*> ULimenGridSpawner::GetNodes() const
{
	TArray<UChildActorComponent*> Result;
	Result.Reserve(GridNodes.Num());
	for (auto& Node : GridNodes) Result.Push(Node.Get());
	return Result;
}

UChildActorComponent* ULimenGridSpawner::GetNodeAt(const int32 Index) const
{
	if (!GridNodes.IsValidIndex(Index)) return nullptr;
	return GridNodes[Index].Get();
}

UChildActorComponent* ULimenGridSpawner::GetNodeByCoordinate(const FIntVector2& Coordinates) const
{
	const int32 Index = Coordinates.X + Coordinates.Y * GridX;
	return GetNodeAt(Index);
}

AActor* ULimenGridSpawner::GetNodeActorByCoordinate(const FIntVector2& Coordinates) const
{
	if (auto* Component = GetNodeByCoordinate(Coordinates))
	{
		if (!Component->GetChildActor()) Component->CreateChildActor();
		return Component->GetChildActor();
	}
	return nullptr;
}

void ULimenGridSpawner::GridCreated(const TArray<UChildActorComponent*>& InGridNode)
{
}

void ULimenGridSpawner::CleanupGrid()
{
	for (TWeakObjectPtr<UChildActorComponent>& GridNode : GridNodes)
	{
		if (TStrongObjectPtr<UChildActorComponent> GridNodeStrongPtr = GridNode.Pin(); GridNodeStrongPtr.IsValid())
		{
			GridNodeStrongPtr->UnregisterComponent();
			GridNodeStrongPtr->DestroyComponent();
			GridNodeStrongPtr->RemoveFromRoot();
			GridNodeStrongPtr->MarkAsGarbage();
			GridNodeStrongPtr.Reset();
		}
		GridNode.Reset();
	}
	GridNodes.Empty();
}

void ULimenGridSpawner::CreateGrid()
{
	if (!CanCreateGrid()) return;

	CleanupGrid();
	const int32 TotalGridSize = GridX * GridY;
	GridNodes.Reserve(TotalGridSize);

	for (int32 Y = 0; Y < GridY; ++Y)
	{
		for (int X = 0; X < GridX; ++X)
		{
			const FName NodeName = FName(*FString::Printf(TEXT("GridNode_%d_%d"), X, Y));
			UChildActorComponent* Node = NewObject<UChildActorComponent>(GetOwner(), NodeName, RF_Transient);
			check(Node)
			const FVector Location = FVector(X * GridNodeOffset.X, Y * GridNodeOffset.Y, 0.f);

			Node->SetRelativeLocation(Location);
			Node->SetupAttachment(this);
			Node->SetChildActorClass(GridNodeActorClass);
			Node->RegisterComponent();

			GridNodes.Push(TWeakObjectPtr(Node));
		}
	}

	GridCreated(GetNodes());
}
