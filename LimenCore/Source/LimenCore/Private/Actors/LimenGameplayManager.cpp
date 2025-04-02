// Copyright Face Software. All Rights Reserved.


#include "Actors/LimenGameplayManager.h"

#include "EngineUtils.h"
#include "TimerManager.h"
#include "BlueprintLibraries/LimenCoreStatics.h"
#include "Engine/Engine.h"
#include "GameFramework/GameModeBase.h"


ALimenGameplayManager* ALimenGameplayManager::GetGameplayManager(UObject* WorldContextObject,
                                                                 const TSubclassOf<ALimenGameplayManager> ManagerClass)
{
	check(GEngine != nullptr)
	const auto* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::Assert);
	for (const TActorIterator It(World, ManagerClass); It;)
	{
		ALimenGameplayManager* Actor = *It;
		return Actor;
	}

	return nullptr;
}

ALimenGameplayManager::ALimenGameplayManager(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bTickEvenWhenPaused = true;
	PrimaryActorTick.bCanEverTick = true;
	bHasInitialized = false;
}

TArray<ALimenGameplayManager*> ALimenGameplayManager::GetGameplayManagers(const UWorld* World, const TSubclassOf<ALimenGameplayManager>& ManagerClass)
{
	check(ManagerClass)
	check(World)

	TArray<ALimenGameplayManager*> OutManagers;
	for (TActorIterator It(World, ManagerClass); It; ++It)
	{
		ALimenGameplayManager* Manager = *It;
		OutManagers.Add(Manager);
	}

	return OutManagers;
}

void ALimenGameplayManager::BeginPlay()
{	
	Super::BeginPlay();

	if (!CanStart())
	{
		ULimenCoreStatics::LimenLog(this, TEXT("Error initializing manager"), ELogType::Error);
		FTimerDelegate DestroyDelegate;
		DestroyDelegate.BindLambda([this]
		{
			verify(Destroy());
		});
		
		GetWorld()->GetTimerManager().SetTimerForNextTick(DestroyDelegate);
	}
	else
	{
		Initialize();
	}
}

void ALimenGameplayManager::Initialize()
{
	Start();
	BP_Start();
	OnObjectInitialized.Broadcast(this);
	bHasInitialized = true;
}

void ALimenGameplayManager::Start()
{
}

bool ALimenGameplayManager::CanStart()
{
	const TArray<ALimenGameplayManager*> Managers = GetGameplayManagers(GetWorld(), GetClass());
	return Managers.Num() == 1 && IsValid(GetWorld()) && GetWorld()->IsGameWorld();
}

void ALimenGameplayManager::BindGameMode(AGameModeBase* InGameMode)
{
	GameModeWeakPtr = InGameMode;
}

bool ALimenGameplayManager::HasInitialized() const
{
	return bHasInitialized;
}

AGameModeBase* ALimenGameplayManager::GetGameMode() const
{
	return GameModeWeakPtr.Get();
}
