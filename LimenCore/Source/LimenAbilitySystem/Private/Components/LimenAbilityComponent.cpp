// Copyright Face Software. All Rights Reserved.


#include "Components/LimenAbilityComponent.h"

#include "Abilities/LimenAbilityBase.h"
#include "Attributes/LimenAttributeBase.h"
#include "Engine/ActorChannel.h"
#include "Engine/Engine.h"
#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"


ULimenAbilityComponent::ULimenAbilityComponent(const FObjectInitializer& InObjectInitializer) : Super(InObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
	SetIsReplicated(true);
	bAutoActivate = true;
	bReplicateUsingRegisteredSubObjectList = true;

	bAbilitiesLoaded = false;
	bAttributesLoaded = false;

	AuthAbilityCount = -1;
	AuthAttributeCount = -1;
}

void ULimenAbilityComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, Abilities,
		FDoRepLifetimeParams(COND_None, REPNOTIFY_OnChanged, true))
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, Attributes,
		FDoRepLifetimeParams(COND_None, REPNOTIFY_OnChanged, true))
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, AuthAbilityCount,
		FDoRepLifetimeParams(COND_None, REPNOTIFY_OnChanged, true))
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, AuthAttributeCount,
		FDoRepLifetimeParams(COND_None, REPNOTIFY_OnChanged, true))
}

void ULimenAbilityComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (GetOwner()->HasAuthority())
	{
		for (FAbilityArrayItem& Ability : Abilities.Items)
		{
			Ability->Deinitialize(GetOwner());
			RemoveReplicatedSubObject(*Ability);
			Ability = nullptr;
		}
		Abilities->Empty();
		Abilities.MarkArrayDirty();

		for (FAttributeArrayItem& Attribute : Attributes.Items)
		{
			Attribute->Deinitialize(GetOwner());
			RemoveReplicatedSubObject(*Attribute);
			Attribute = nullptr;
		}
		Attributes->Empty();
		Attributes.MarkArrayDirty();
	}
	
	Super::EndPlay(EndPlayReason);
}

void ULimenAbilityComponent::Deactivate()
{
	DeactivateAllAbilities();

	Super::Deactivate();
}

void ULimenAbilityComponent::LoadAbilities(AActor* Owner)
{
	check(GetOwner()->HasAuthority())

	Abilities->Empty(AbilityClasses.Num());
	for (auto& AbilityClass : AbilityClasses)
	{
		check(!AbilityClass.IsNull());
		
		ULimenAbilityBase* Ability = NewObject<ULimenAbilityBase>(this, AbilityClass.LoadSynchronous());
		AddReplicatedSubObject(Ability);
		Ability->Initialize(Owner);

		const int32 Index = Abilities->Add(FAbilityArrayItem(Ability));
		Abilities.MarkItemDirty(Abilities[Index]);
	}
	bAbilitiesLoaded = true;
	AuthAbilityCount = Abilities->Num();

	if (bAttributesLoaded)
	{
		OnAbilityComponentReady.Broadcast(this);
	}
}

void ULimenAbilityComponent::LoadAttributes(AActor* Owner)
{
	check(GetOwner()->HasAuthority())

	Attributes->Empty(AttributeClasses.Num());
	for (auto& AttributeClass : AttributeClasses)
	{
		check(!AttributeClass.IsNull());
		
		ULimenAttributeBase* Attribute = NewObject<ULimenAttributeBase>(this,
			AttributeClass.LoadSynchronous());
		AddReplicatedSubObject(Attribute);
		
		Attribute->Initialize(Owner);

		const int32 Index = Attributes->Add(FAttributeArrayItem(Attribute));
		Attributes.MarkItemDirty(Attributes[Index]);
	}
	bAttributesLoaded = true;
	AuthAttributeCount = Attributes->Num();

	if (bAbilitiesLoaded)
	{
		OnAbilityComponentReady.Broadcast(this);
	}
}

void ULimenAbilityComponent::AddAbility(const TSubclassOf<ULimenAbilityBase>& AbilityClass)
{
	check(AbilityClass != nullptr);
	
	AbilityClasses.Push(TSoftClassPtr<ULimenAbilityBase>(AbilityClass));

	ULimenAbilityBase* Ability = NewObject<ULimenAbilityBase>(this, AbilityClass);
	const int32 Index = Abilities->Add(FAbilityArrayItem(Ability));
	Abilities.MarkItemDirty(Abilities[Index]);
}

void ULimenAbilityComponent::AddAttribute(const TSubclassOf<ULimenAttributeBase>& AttributeClass)
{
	check(AttributeClass != nullptr);
	
	AttributeClasses.Push(TSoftClassPtr<ULimenAttributeBase>(AttributeClass));

	ULimenAttributeBase* Attribute = NewObject<ULimenAttributeBase>(this, AttributeClass);
	const int32 Index = Attributes->Add(FAttributeArrayItem(Attribute));
	Attributes.MarkItemDirty(Attributes[Index]);
}

bool ULimenAbilityComponent::IsReadyForGameplay() const
{
	return bAbilitiesLoaded && bAttributesLoaded;
}

void ULimenAbilityComponent::DeactivateAllAbilities()
{
	check(GetOwner()->HasAuthority())
	for (const FAbilityArrayItem& Ability : Abilities.Items)
	{
		check(*Ability)
		Ability->ForceDeactivateAbility();
	}
}

ULimenAbilityBase* ULimenAbilityComponent::GetAbility(const TSubclassOf<ULimenAbilityBase> AbilityClass) const
{
	for (const FAbilityArrayItem& Ability : Abilities.Items)
	{
		if (*Ability == nullptr)
		{
			continue;
		}

		if (Ability->IsA(AbilityClass))
		{
			return *Ability;
		}
	}

	return nullptr;
}

ULimenAttributeBase* ULimenAbilityComponent::GetAttribute(const TSubclassOf<ULimenAttributeBase> AttributeClass) const
{
	for (const FAttributeArrayItem& Attribute : Attributes.Items)
	{
		if (*Attribute == nullptr)
		{
			continue;
		}

		if (Attribute->GetClass() == AttributeClass || Attribute->GetClass()->IsChildOf(AttributeClass))
		{
			return *Attribute;
		}
	}

	return nullptr;
}

TArray<ULimenAbilityBase*> ULimenAbilityComponent::GetAbilities() const
{
	TArray<ULimenAbilityBase*> Out;
	Out.Reserve(Abilities->Num());

	for (const FAbilityArrayItem& Ability : Abilities.Items)
	{
		Out.Push(*Ability);
	}

	return Out;
}

TArray<ULimenAttributeBase*> ULimenAbilityComponent::GetAttributes() const
{
	TArray<ULimenAttributeBase*> Out;
	Out.Reserve(Attributes->Num());

	for (const FAttributeArrayItem& Attribute : Attributes.Items)
	{
		Out.Push(*Attribute);
	}

	return Out;
}

void ULimenAbilityComponent::OnRep_ServerAbilityCount()
{
	bAbilitiesLoaded = Abilities->Num() == AuthAbilityCount;
	if (IsNoAuthComponentGameplayReady())
	{
		OnAbilityComponentReady.Broadcast(this);
	}
}

void ULimenAbilityComponent::OnRep_ServerAttributeCount()
{
	bAttributesLoaded = Attributes->Num() == AuthAttributeCount;
	if (IsNoAuthComponentGameplayReady())
	{
		OnAbilityComponentReady.Broadcast(this);
	}
}

bool ULimenAbilityComponent::IsNoAuthComponentGameplayReady()
{
	check(!GetOwner()->HasAuthority())
	return Abilities->Num() == AuthAbilityCount && Attributes->Num() == AuthAttributeCount;
}
