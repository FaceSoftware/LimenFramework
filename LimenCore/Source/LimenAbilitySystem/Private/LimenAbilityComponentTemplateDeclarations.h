// Copyright Face Software. All Rights Reserved.

#pragma once

#include "Abilities/LimenAbilityBase.h"
#include "Attributes/LimenAttributeBase.h"
#include "Components/LimenAbilityComponent.h"


template <typename AbilityClass>
void ULimenAbilityComponent::InitializeAbilityDependency()
{
	static_assert(std::is_base_of_v<ULimenAbilityBase, AbilityClass>);
	if (ULimenAbilityBase* Ability = GetAbility<AbilityClass>(); Ability != nullptr)
	{
		if (!Ability->IsInitialized())
		{
			Ability->Initialize(Cast<APawn>(GetOwner()));
		}
	}
}

template <typename AttributeClass>
void ULimenAbilityComponent::InitializeAttributeDependency()
{
	static_assert(std::is_base_of_v<ULimenAttributeBase, AttributeClass>);
	if (ULimenAttributeBase* Attribute = GetAbility<AttributeClass>(); Attribute != nullptr)
	{
		if (!Attribute->IsInitialized())
		{
			Attribute->Initialize(Cast<APawn>(GetOwner()));
		}
	}
}