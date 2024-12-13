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
	for (TStrongObjectPtr<ULimenAbilityBase>& Ability : Abilities)
	{
		Ability.Reset();
	}
	Abilities.Empty();

	for (TStrongObjectPtr<ULimenAttributeBase>& Attribute : Attributes)
	{
		Attribute.Reset();
	}
	Attributes.Empty();
	
	Super::EndPlay(EndPlayReason);
}

void ULimenAbilityComponent::LoadAbilities(AActor* Owner)
{
	Abilities.Empty(AbilityClasses.Num());
	
	for (auto& AbilityClass : AbilityClasses)
	{
		check(!AbilityClass.IsNull());
		
		ULimenAbilityBase* Temp = NewObject<ULimenAbilityBase>(this, AbilityClass.LoadSynchronous());
		Temp->Initialize(Owner);
		Abilities.Push(TStrongObjectPtr(Temp));
	}

	bAbilitiesLoaded = true;
}

void ULimenAbilityComponent::LoadAttributes(AActor* Owner)
{
	Attributes.Empty(AttributeClasses.Num());
	for (auto& AttributeClass : AttributeClasses)
	{
		check(!AttributeClass.IsNull());
		
		ULimenAttributeBase* Temp = NewObject<ULimenAttributeBase>(this, AttributeClass.LoadSynchronous());
		Temp->Initialize(Owner);
		Attributes.Push(TStrongObjectPtr(Temp));
	}

	bAttributesLoaded = true;
}

void ULimenAbilityComponent::AddAbility(const TSubclassOf<ULimenAbilityBase>& AbilityClass)
{
	check(AbilityClass != nullptr);
	
	AbilityClasses.Push(TSoftClassPtr<ULimenAbilityBase>(AbilityClass));
	
	ULimenAbilityBase* Ability = NewObject<ULimenAbilityBase>(this, AbilityClass);
	Abilities.Push(TStrongObjectPtr(Ability));
}

void ULimenAbilityComponent::AddAttribute(const TSubclassOf<ULimenAttributeBase>& AttributeClass)
{
	check(AttributeClass != nullptr);
	
	AttributeClasses.Push(TSoftClassPtr<ULimenAttributeBase>(AttributeClass));
	
	ULimenAttributeBase* Attribute = NewObject<ULimenAttributeBase>(this, AttributeClass);
	Attributes.Push(TStrongObjectPtr(Attribute));
}

bool ULimenAbilityComponent::IsReadyForGameplay() const
{
	return bAbilitiesLoaded && bAttributesLoaded;
}

void ULimenAbilityComponent::DeactivateAllAbilities()
{
	for (const TStrongObjectPtr<ULimenAbilityBase>& Ability : Abilities)
	{
		check(Ability.IsValid())
		Ability->ForceDeactivateAbility();
	}
}

ULimenAbilityBase* ULimenAbilityComponent::GetAbility(const TSubclassOf<ULimenAbilityBase> AbilityClass)
{
	for (const TStrongObjectPtr<ULimenAbilityBase>& Ability : Abilities)
	{
		check(Ability.IsValid())
		if (Ability->GetClass() == AbilityClass)
		{
			return Ability.Get();
		}
	}

	return nullptr;
}

ULimenAttributeBase* ULimenAbilityComponent::GetAttribute(const TSubclassOf<ULimenAttributeBase> AttributeClass)
{
	for (const TStrongObjectPtr<ULimenAttributeBase>& Attribute : Attributes)
	{
		if (Attribute->GetClass() == AttributeClass || Attribute->GetClass()->IsChildOf(AttributeClass))
		{
			return Attribute.Get();
		}
	}

	return nullptr;
}

TArray<ULimenAbilityBase*> ULimenAbilityComponent::GetAbilities() const
{
	TArray<ULimenAbilityBase*> Out;
	Out.Reserve(Abilities.Num());
	
	for (auto& Ability : Abilities)
	{
		Out.Push(Ability.Get());
	}
	
	return Out;
}

TArray<ULimenAttributeBase*> ULimenAbilityComponent::GetAttributes() const
{
	TArray<ULimenAttributeBase*> Out;
	Out.Reserve(Attributes.Num());
	
	for (auto& Attribute : Attributes)
	{
		Out.Push(Attribute.Get());
	}
	
	return Out;
}
