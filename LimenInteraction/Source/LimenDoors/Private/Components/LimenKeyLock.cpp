// Copyright Face Software. All Rights Reserved.


#include "Components/LimenKeyLock.h"

#include "BlueprintLibraries/LimenCoreStatics.h"
#include "Keys/LimenKey.h"


ULimenKeyLock::ULimenKeyLock()
{
	bIsLocked = false;
	bUsesKey = false;
	bIsLocked = false;
}

void ULimenKeyLock::OnComponentCreated()
{
	Super::OnComponentCreated();

	bIsLocked ? Lock() : Unlock();
}

void ULimenKeyLock::BeginPlay()
{
	Super::BeginPlay();
	SetKey();

	if (!Key && bUsesKey)
	{
		const FString Message = FString::Printf(TEXT("Invalid key for door which is set to use key: %s"), *GetOwner()->GetName());
		ULimenCoreStatics::LimenLog(this, Message, ELogType::Error);
	}
}

bool ULimenKeyLock::Unlock(const ALimenKey* TestKey)
{
	if (!IsValid(TestKey) || TestKey != Key)
	{
		return false;
	}

	Unlock();
	return true;
}

bool ULimenKeyLock::IsLocked() const
{
	return bIsLocked;
}

void ULimenKeyLock::Lock()
{
	bIsLocked = true;
}

void ULimenKeyLock::Unlock()
{
	bIsLocked = false;
}
