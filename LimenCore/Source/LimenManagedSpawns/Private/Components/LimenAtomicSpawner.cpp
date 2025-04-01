// Copyright Face Software. All Rights Reserved.


#include "Components/LimenAtomicSpawner.h"

#include "Kismet/KismetMathLibrary.h"
#include "LimenCore/Public/Subsystems/LimenGlobalRandomStreamSubsystem.h"


ULimenAtomicSpawner::ULimenAtomicSpawner()
{
	PrimaryComponentTick.bCanEverTick = false;

	SpawnParams.Instigator = nullptr;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	BodyInstance.SetResponseToAllChannels(ECR_Ignore);
	BodyInstance.SetCollisionEnabled(ECollisionEnabled::Type::NoCollision, false);
}

void ULimenAtomicSpawner::SpawnItem(const TSubclassOf<AActor>& Class, const int32 Amount)
{
	for (int i = 0; i < Amount; ++i)
	{
		const FBoxSphereBounds SpawnBounds = GetLocalBounds().TransformBy(GetComponentTransform());

		FRandomStreamRef RandomStream = ULimenGlobalRandomStreamSubsystem::Get()->GetGlobalRandomStream();
		
		const FVector SpawnLocation = SpawnBounds.Origin + UKismetMathLibrary::RandomPointInBoundingBoxFromStream(RandomStream.Get(), FVector::ZeroVector, SpawnBounds.BoxExtent);
		
		AActor* SpawnedActor = GetWorld()->SpawnActor(Class, &SpawnLocation, &FRotator::ZeroRotator, SpawnParams);

		check(SpawnedActor != nullptr);
		SpawnedActors.Push(SpawnedActor);

		SnapToFloor(SpawnedActor);
	}
}

bool ULimenAtomicSpawner::IsTagCompatible(const FName& Test) const
{
	if (ComponentTags.IsEmpty() && Test.IsNone())
	{
		return true;
	}

	return ComponentTags.Contains(Test);
}

int32 ULimenAtomicSpawner::GetSpawnedItemsCount() const
{
	return SpawnedActors.Num();
}

void ULimenAtomicSpawner::SnapToFloor(AActor* Item) const
{
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(Item);
	QueryParams.bTraceComplex = true;
	QueryParams.bIgnoreBlocks = false;
	QueryParams.bIgnoreTouches = false;
	QueryParams.MobilityType = EQueryMobilityType::Any;

	const FCollisionResponseParams ResponseParams(ECollisionResponse::ECR_Block);

	const FVector Start = Item->GetActorLocation();
	const FVector End = Start + (FVector::DownVector * 1000);

	if (FHitResult Hit;
		GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Camera, QueryParams, ResponseParams))
	{
		const FBox BoundingBox = Item->GetComponentsBoundingBox(false, true);
		const FVector SnappedLocation = Hit.Location + FVector(0.0, 0.0, BoundingBox.GetExtent().Z + UE_DOUBLE_KINDA_SMALL_NUMBER);
		Item->SetActorLocation(SnappedLocation, false, nullptr, ETeleportType::ResetPhysics);
	}

	/*
	if (FHitResult Hit;
		GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Camera, QueryParams, ResponseParams))
	{
		const FVector SnappedLocation = Hit.Location + Hit.ImpactNormal * 0.1f;
		Item->SetActorLocation(SnappedLocation, false, nullptr, ETeleportType::ResetPhysics);
		// Item->SetActorLocationAndRotation(SnappedLocation, Hit.ImpactNormal.ToOrientationRotator(), false, nullptr, ETeleportType::ResetPhysics);
	}
	*/
}

ALimenAtomicSpawnerActor::ALimenAtomicSpawnerActor(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
	
	AtomicSpawnerComponent = CreateDefaultSubobject<ULimenAtomicSpawner>(TEXT("AtomicSpawnerComponent"));
	RootComponent = AtomicSpawnerComponent;
}

