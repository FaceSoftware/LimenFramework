// Copyright Face Software. All Rights Reserved.


#include "GameplayManagers/LimenSaveableHintsManager.h"

#include "Objects/LimenHint.h"
#include "Subsystems/LimenGameSaveSubsystem.h"


void ALimenSaveableHintsManager::BeginPlay()
{
	Super::BeginPlay();
	
	if (const ULimenGameSaveSubsystem* GameSaveSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<ULimenGameSaveSubsystem>();
		!GameSaveSubsystem->HasLoadedGameData())
	{
		SaveHints();
		SetupRemoveStaleHints();
	}
}

bool ALimenSaveableHintsManager::ShouldLoadData() const
{
	return true;
}

void ALimenSaveableHintsManager::PreDataSaved()
{
}

bool ALimenSaveableHintsManager::ShouldSaveData() const
{
	return true;
}

void ALimenSaveableHintsManager::PostDataLoaded()
{
	LoadSavedHints();
	SetupRemoveStaleHints();
}

void ALimenSaveableHintsManager::PostDataSaved()
{
}

void ALimenSaveableHintsManager::PreDataLoaded()
{
}

void ALimenSaveableHintsManager::LoadSavedHints()
{
	HintClasses.Empty(SavedHintClasses.Num());
	for (auto& ClassPath : SavedHintClasses)
	{
		TSoftClassPtr<ULimenHint> HintClass(ClassPath.ResolveClass());
		check(!HintClass.IsNull());
		HintClasses.Push(HintClass);
	}
	
	InitializeHints();
	
	for (ULimenHint* Hint : Hints)
	{
		Hint->OnHintDismissed.AddLambda([this, Hint]
		{
			SavedHintClasses.Remove(FSoftClassPath(Hint->GetClass()));
		});
	}
}

void ALimenSaveableHintsManager::SaveHints()
{
	SavedHintClasses.Empty(HintClasses.Num());
	for (auto& HintClass : HintClasses)
	{
		check(!HintClass.IsNull());
		FSoftClassPath HintClassPath(HintClass.LoadSynchronous());
		SavedHintClasses.Push(HintClassPath);
	}
	
	InitializeHints();
}

void ALimenSaveableHintsManager::SetupRemoveStaleHints()
{
	for (ULimenHint* Hint : Hints)
	{
		Hint->OnHintDismissed.AddLambda([this, Hint]
		{
			SavedHintClasses.Remove(FSoftClassPath(Hint->GetClass()));
		});
	}
}
