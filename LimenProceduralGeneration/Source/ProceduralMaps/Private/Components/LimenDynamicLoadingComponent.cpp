// Copyright Face Software. All Rights Reserved.


#include "Components/LimenDynamicLoadingComponent.h"

#include "Actors/LimenGameplayManager.h"
#include "Actors/LimenProceduralTileMapBuilder.h"
#include "Kismet/GameplayStatics.h"
#include "Threading/ThreadedFunction.h"


ULimenDynamicLoadingComponent::ULimenDynamicLoadingComponent()
{
	TileUnloadDistance = 5000.f;
	bIsLoaded = true;
}

void ULimenDynamicLoadingComponent::BeginPlay()
{
	Super::BeginPlay();
	
	LoadActor();

	FTimerDelegate NextTickTimer; NextTickTimer.BindLambda([this]
	{
		if (!IsValid(GetWorld()))
		{
			return;
		}
		
		if (auto* MapBuilder = ALimenGameplayManager::GetGameplayManager<ALimenProceduralTileMapBuilder>(GetWorld()); IsValid(MapBuilder))
		{
			MapBuilder->OnBuildProcessFinished.AddDynamic(this, &ThisClass::UpdateLoadState);
		}
	});
	
	GetWorld()->GetTimerManager().SetTimerForNextTick(NextTickTimer);
}

bool ULimenDynamicLoadingComponent::IsLoaded()
{
	FScopeLock Lock(&LoadedSection);
	return bIsLoaded;
}

void ULimenDynamicLoadingComponent::SetTileUnloadDistance(const float NewUnloadDistance)
{
	TileUnloadDistance = NewUnloadDistance;
}

void ULimenDynamicLoadingComponent::LoadActor()
{
	LoadedSection.Lock();
	bIsLoaded = true;
	LoadedSection.Unlock();

	AsyncTask(ENamedThreads::GameThread, [this]
	{
		GetOwner()->SetActorHiddenInGame(false);
	});
}

void ULimenDynamicLoadingComponent::UnloadActor()
{
	LoadedSection.Lock();
	bIsLoaded = false;
	LoadedSection.Unlock();

	AsyncTask(ENamedThreads::GameThread, [this]
	{
		GetOwner()->SetActorHiddenInGame(true);
	});
}

bool ULimenDynamicLoadingComponent::ShouldLoad()
{
	const FVector TargetLocation = UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->GetActorLocation();;
	const FVector OwnerLocation = GetOwner()->GetActorLocation();
	
	const float Distance = FVector::Distance(TargetLocation, OwnerLocation);
	return Distance < TileUnloadDistance;
}

void ULimenDynamicLoadingComponent::UpdateLoadState()
{
	const auto* Pawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!IsValid(Pawn))
	{
		return;
	}

	LoadHandler();
}

void ULimenDynamicLoadingComponent::LoadHandler()
{
	if (ShouldLoad() && !IsLoaded())
	{
		LoadActor();
	}
	else if (!ShouldLoad() && IsLoaded())
	{
		UnloadActor();
	}
}
