// Copyright Face Software. All Rights Reserved.


#include "Components/LimenLock.h"

#include "Keys/LimenKey.h"


ULimenLock::ULimenLock(const FObjectInitializer& ObjectInitializer)
{
	bAutoActivate = true;
	bStartLocked = false;
	bIsLocked = bStartLocked;
}

void ULimenLock::BeginPlay()
{
	Super::BeginPlay();
	
	bIsLocked = bStartLocked && IsActive();
}

void ULimenLock::SetKeyword(const FName& NewKeyword)
{
	Keyword = NewKeyword;
}

FName ULimenLock::GetKeyword() const
{
	return Keyword;
}

bool ULimenLock::IsLocked() const
{
	if (!IsActive())
	{
		return false;
	}
	return bIsLocked;
}

void ULimenLock::SetLockedState(const bool bLocked)
{
	bIsLocked = bLocked;
}

bool ULimenLock::ChangeLockedState(AController* Controller, APawn* Pawn, ALimenKey* Test, const bool bLock)
{
	check(Test != nullptr);
	
	if (bLock == bIsLocked)
	{
		return true;
	}

	if (!IsActive())
	{
		return false;
	}
	
	if (!Keyword.IsNone() && (Test == nullptr || Test->GetKeyword() != Keyword))
	{
		return false;
	}
	
	bIsLocked = bLock;
	bIsLocked ? OnDoorLocked.Broadcast(Controller, Pawn, Test)
			  : OnDoorUnlocked.Broadcast(Controller, Pawn, Test);

	return true;
}

bool ULimenLock::ChangeLockedStateWithKeys(AController* Controller, APawn* Pawn, TArray<ALimenKey*> Test,
	const bool bLock)
{
	for (ALimenKey* Key : Test)
	{
		check(Key != nullptr);
		if (!ChangeLockedState(Controller, Pawn, Key, bLock))
		{
			continue;
		}

		return true;
	}

	return false;
}

bool ULimenLock::TryKey(ALimenKey* Test) const
{
	check(Test != nullptr);
	if (!IsActive())
	{
		return false;
	}
	
	return Test->GetKeyword() == Keyword;
}
