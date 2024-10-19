// Copyright Face Software. All Rights Reserved.


#include "Components/LimenAbilityComponent.h"

#include "Abilities/LimenAbilityBase.h"
#include "Attributes/LimenAttributeBase.h"


ULimenAbilityComponent::ULimenAbilityComponent(const FObjectInitializer& InObjectInitializer) : Super(InObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;

	bAbilitiesLoaded = false;
	bAttributesLoaded = false;
}

void ULimenAbilityComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	for (ULimenAbilityBase* Ability : Abilities)
	{
		Ability->ConditionalBeginDestroy();
	}
	Abilities.Empty();

	for (ULimenAttributeBase* Attribute : Attributes)
	{
		Attribute->ConditionalBeginDestroy();
	}
	Attributes.Empty();
}

void ULimenAbilityComponent::LoadAbilities(AActor* Owner)
{
	Abilities.Empty(AbilityClasses.Num());
	for (auto& AbilityClass : AbilityClasses)
	{
		check(!AbilityClass.IsNull());
	
		TObjectPtr<ULimenAbilityBase> Temp = NewObject<ULimenAbilityBase>(this, AbilityClass.LoadSynchronous());
		check(Temp != nullptr);
		Temp->Initialize(Owner);
		Abilities.Push(Temp);
	}

	bAbilitiesLoaded = true;
}

void ULimenAbilityComponent::LoadAttributes(AActor* Owner)
{
	Attributes.Empty(AttributeClasses.Num());
	for (auto& AttributeClass : AttributeClasses)
	{
		check(!AttributeClass.IsNull());
		
		TObjectPtr<ULimenAttributeBase> Temp = NewObject<ULimenAttributeBase>(this, AttributeClass.LoadSynchronous());
		check(Temp != nullptr);
		Temp->Initialize(Owner);
		Attributes.Push(Temp);
	}

	bAttributesLoaded = true;
}

void ULimenAbilityComponent::AddAbility(const TSubclassOf<ULimenAbilityBase>& AbilityClass)
{
	check(AbilityClass != nullptr);
	AbilityClasses.Push(TSoftClassPtr<ULimenAbilityBase>(AbilityClass));
	ULimenAbilityBase* Ability = NewObject<ULimenAbilityBase>(this, AbilityClass);
	Abilities.Push(Ability);
}

void ULimenAbilityComponent::AddAttribute(const TSubclassOf<ULimenAttributeBase>& AttributeClass)
{
	check(AttributeClass != nullptr);
	AttributeClasses.Push(TSoftClassPtr<ULimenAttributeBase>(AttributeClass));
	ULimenAttributeBase* Attribute = NewObject<ULimenAttributeBase>(this, AttributeClass);
	Attributes.Push(Attribute);
}

bool ULimenAbilityComponent::IsReadyForGameplay() const
{
	return bAbilitiesLoaded && bAttributesLoaded;
}

void ULimenAbilityComponent::DeactivateAllAbilities()
{
	for (ULimenAbilityBase* Ability : Abilities)
	{
		check(Ability != nullptr)
		Ability->ForceDeactivateAbility();
	}
}

ULimenAbilityBase* ULimenAbilityComponent::GetAbility(const TSubclassOf<ULimenAbilityBase> AbilityClass)
{
	for (ULimenAbilityBase* Ability : Abilities)
	{
		if (Ability->GetClass() == AbilityClass)
		{
			return Ability;
		}
	}

	return nullptr;
}

ULimenAttributeBase* ULimenAbilityComponent::GetAttribute(const TSubclassOf<ULimenAttributeBase> AttributeClass)
{
	for (ULimenAttributeBase* Attribute : Attributes)
	{
		if (Attribute->GetClass() == AttributeClass || Attribute->GetClass()->IsChildOf(AttributeClass))
		{
			return Attribute;
		}
	}

	return nullptr;
}

const TArray<ULimenAbilityBase*>& ULimenAbilityComponent::GetAbilities()
{
	return Abilities;
}

const TArray<ULimenAttributeBase*>& ULimenAbilityComponent::GetAttributes()
{
	return Attributes;
}
