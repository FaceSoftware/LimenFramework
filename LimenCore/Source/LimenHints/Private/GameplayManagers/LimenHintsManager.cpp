// Copyright Face Software. All Rights Reserved.


#include "GameplayManagers/LimenHintsManager.h"

#include "Objects/LimenHint.h"
#include "UMG/LimenHintWidget.h"


ALimenHintsManager::ALimenHintsManager()
{
	bHintsEnabled = false;
}

void ALimenHintsManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	Hints.Empty();
}

void ALimenHintsManager::SetHintWidgetClass(const TSubclassOf<ULimenHintWidget>& InClass)
{
	HintWidgetClass = InClass;
}

void ALimenHintsManager::InitializeHints()
{
	ensureAlways(!HintWidgetClass.IsNull());
	
	Hints.Reserve(HintClasses.Num());
	for (TSoftClassPtr<ULimenHint>& HintClass : HintClasses)
	{
		check(!HintClass.IsNull());

		ULimenHint* Hint = NewObject<ULimenHint>(this, HintClass.LoadSynchronous());
		check(IsValid(Hint));		
		Hints.Push(TStrongObjectPtr(Hint));
	}	
	
	check(HintWidgetClass != nullptr);
	for (const TStrongObjectPtr<ULimenHint>& Hint : Hints)
	{
		Hint->SetHintWidgetClass(HintWidgetClass.LoadSynchronous());
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

	for (const auto& Hint : Hints)
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

	for (const auto& Hint : Hints)
	{
		Hint->Disable();
	}
	
	bHintsEnabled = false;
}

bool ALimenHintsManager::AreHintsEnabled() const
{
	return bHintsEnabled;
}
