// Copyright Face Software. All Rights Reserved.


#include "Components/LimenCreditsComponent.h"


ULimenCreditsComponent::ULimenCreditsComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	StartingCredits = 0;
	CurrentCredits = 0;
}

void ULimenCreditsComponent::OnComponentCreated()
{
	Super::OnComponentCreated();

	CurrentCredits = StartingCredits;
}

void ULimenCreditsComponent::AddCredits(const float Amount)
{
	if (Amount < 0)
	{
		return;
	}
	
	CurrentCredits += Amount;
	OnCreditsCountUpdated.Broadcast(CurrentCredits, Amount);
}

void ULimenCreditsComponent::RemoveCredits(const float Amount)
{
	if (Amount < 0)
	{
		return;
	}
	
	CurrentCredits -= Amount;
	OnCreditsCountUpdated.Broadcast(CurrentCredits, -Amount);
}

float ULimenCreditsComponent::GetCredits() const
{
	return CurrentCredits;
}

FString ULimenCreditsComponent::GetCreditsString() const
{
	return FString::Printf(TEXT("%f %s"), CurrentCredits, *CreditsName.ToString());
}

FString ULimenCreditsComponent::GetCreditsName() const
{
	return CreditsName.ToString();
}

bool ULimenCreditsComponent::HasCredits(const float Amount) const
{
	return CurrentCredits >= Amount;
}
