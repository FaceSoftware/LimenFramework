// Copyright FaceSoftware. All Rights Reserved.


#include "Components/LimenCommandComponent.h"


ULimenCommandComponent::ULimenCommandComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	bAutoActivate = true;
}

void ULimenCommandComponent::ExecuteCommand(const FName& Command, const TArray<FName>& Args, UObject* Requestor)
{
	if (IsActive())
	{
		OnCommandRequested.Broadcast(Command, Args, Requestor);
	}
}

void ULimenCommandComponent::ExecuteCommand(const FName& Command, const FName& Arg, UObject* Requestor)
{
	ExecuteCommand(Command, TArray({ Arg }), Requestor);
}
