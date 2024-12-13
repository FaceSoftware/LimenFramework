// Copyright Face Software. All Rights Reserved.


#include "Attributes/LimenStaminaAttribute.h"

#include "Components/LimenAbilityComponent.h"

ULimenStaminaAttribute::ULimenStaminaAttribute()
{
	MaxValue = 100.f;
	InitialValue = 100.f;
	RechargeRate = 5.f;
	DevelopmentName = TEXT("att_stamina");
	DisplayName = FText::FromString(TEXT("Stamina"));
	Category = FText::FromString(TEXT("Player"));
}

void ULimenStaminaAttribute::Initialize(AActor* InOwner)
{
	Super::Initialize(InOwner);
}
