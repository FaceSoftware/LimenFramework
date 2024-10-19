// Copyright Face Software. All Rights Reserved.


#include "Components/LimenConeTraceComponent.h"

#include "Namespaces/LimenCoreMath.h"


ULimenConeTraceComponent::ULimenConeTraceComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.015625;
	ConeData = FConeData();

	LineTraceCollisionChannel = static_cast<ECollisionChannel>(0);
}

void ULimenConeTraceComponent::BeginPlay()
{
	Super::BeginPlay();

	IgnoredActors.Empty(2);
	IgnoredActors.Push(nullptr);
	IgnoredActors.Push(GetOwner());
}

void ULimenConeTraceComponent::TickComponent(float DeltaTime, ELevelTick TickType,
											 FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!IsActive())
	{
		return;
	}

	ActorsToCheck.Remove(nullptr);

	if (ConeData.IsPossible() || !ActorsToCheck.IsEmpty())
	{
		for (auto& Actor : ActorsToCheck)
		{
			if (Actor.Key == nullptr)
			{
				continue;
			}
			
			LineTraceQueryParams.ClearIgnoredActors();
			IgnoredActors[0] = Actor.Key;
			LineTraceQueryParams.AddIgnoredActors(IgnoredActors);
			
			if (const bool bIsActorInCone = IsActorInCone(Actor.Key); bIsActorInCone && !Actor.Value)
			{
				Actor.Value = true;
				OnConeCollisionUpdate.Broadcast(Actor.Key, Actor.Value);
			}
			else if (!bIsActorInCone && Actor.Value)
			{
				Actor.Value = false;
				OnConeCollisionUpdate.Broadcast(Actor.Key, Actor.Value);
			}
		}
	}
}

void ULimenConeTraceComponent::AddLineTraceActorToIgnore(AActor* Actor)
{
	IgnoredActors.Push(Actor);
}

void ULimenConeTraceComponent::AddActorToCheck(AActor* InActor)
{
	ActorsToCheck.Add(InActor, false);
}

void ULimenConeTraceComponent::Activate(bool bReset)
{
	Super::Activate(bReset);
}

void ULimenConeTraceComponent::Deactivate()
{
	Super::Deactivate();

	for (auto& Actor : ActorsToCheck)
	{
		if (Actor.Value)
		{
			Actor.Value = false;
			OnConeCollisionUpdate.Broadcast(Actor.Key, Actor.Value);
		}
	}
}

void ULimenConeTraceComponent::SetConeData(const FConeData& InConeData)
{
	ConeData = InConeData;
}

bool ULimenConeTraceComponent::IsActorInCone(const AActor* Test)
{
	const FVector OwnerLocation = GetOwner()->GetActorLocation();
	const FVector TestLocation = Test->GetActorLocation();
	FVector OwnerToTest = TestLocation - OwnerLocation;

	// Check if enemy is within range
	if (OwnerToTest.Size() > ConeData.GetHeight())
	{
		return false;
	}

	// Get the forward vector (direction the player is facing)
	const FVector ForwardVector = GetOwner()->GetActorForwardVector();

	// Calculate the angle between the player's forward vector and the vector to the enemy
	OwnerToTest.Normalize();
	const float DotProduct = FVector::DotProduct(ForwardVector, OwnerToTest);
	const float Angle = FMath::Acos(DotProduct) * 180.f / PI;

	// Check if the enemy is within the beam angle
	if (Angle >= ConeData.GetAngle() / 2.f)
	{
		return false;
	}

	FHitResult Hit;
	const FVector Start = OwnerLocation;
	const FVector End = Test->GetActorLocation();
	return !GetWorld()->LineTraceSingleByChannel(Hit, Start, End, LineTraceCollisionChannel, LineTraceQueryParams);
}
