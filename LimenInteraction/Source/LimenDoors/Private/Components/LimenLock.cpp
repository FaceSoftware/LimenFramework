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

bool ULimenLock::ChangeLockedState(AController* Controller, APawn* Pawn, FName Test, const bool bLock)
{	
	if (bLock == bIsLocked)
	{
		return true;
	}

	if (!IsActive())
	{
		return false;
	}
	
	if (!TryKeyword(Test)) return false;
	
	bIsLocked = bLock;
	bIsLocked ? OnLocked.Broadcast(Controller, Pawn)
			  : OnUnlocked.Broadcast(Controller, Pawn);

	return true;
}

bool ULimenLock::ChangeLockedState(AController* Controller, APawn* Pawn, const TArray<const FName>& Keywords,
								   const bool bLock)
{
	for (const FName& Test : Keywords)
	{
		if (!ChangeLockedState(Controller, Pawn, Test, bLock)) continue;
		return true;
	}

	return false;
}

bool ULimenLock::TryKeyword(const FName& Test) const
{	
	return Test == Keyword;
}
