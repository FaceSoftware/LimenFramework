// Copyright Face Software. All Rights Reserved.


#include "GameplayManagers/LimenHintsManager.h"

#include "Objects/LimenHint.h"


ALimenHintsManager::ALimenHintsManager()
{
	bHintsEnabled = false;
}

void ALimenHintsManager::InitializeHints()
{
	Hints.Reserve(HintClasses.Num());
	for (TSoftClassPtr<ULimenHint>& HintClass : HintClasses)
	{
		if (HintClass.IsNull())
		{
			continue;
		}

		ULimenHint* Hint = NewObject<ULimenHint>(this, HintClass.LoadSynchronous());
		check(IsValid(Hint));		
		Hints.Push(Hint);
	}	
	
	for (ULimenHint* Hint : Hints)
	{
		Hint->Initialize();
		bHintsEnabled ? Hint->Enable() : Hint->Disable();
	}
}

void ALimenHintsManager::EnableHints()
{
	if (bHintsEnabled)
	{
		return;
	}

	for (auto* Hint : Hints)
	{
		Hint->Enable();
	}
	
	bHintsEnabled = true;
}

void ALimenHintsManager::DisableHints()
{
	if (!bHintsEnabled)
	{
		return;
	}

	for (auto* Hint : Hints)
	{
		Hint->Disable();
	}
	
	bHintsEnabled = false;
}
