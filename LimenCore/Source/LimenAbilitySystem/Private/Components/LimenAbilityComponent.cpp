// Copyright Face Software. All Rights Reserved.


#include "Components/LimenAbilityComponent.h"

#include "Abilities/LimenAbilityBase.h"
#include "Attributes/LimenAttributeBase.h"
#include "Engine/Engine.h"
#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"


ULimenAbilityComponent::ULimenAbilityComponent(const FObjectInitializer& InObjectInitializer) : Super(InObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
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
	Super::EndPlay(EndPlayReason);

	if (GetOwner()->HasAuthority())
	{
		DeactivateAllAbilities();
		DeactivateAllAttributes();

		DestroyAllAbilities();
		DestroyAllAttributes();
	}
}

void ULimenAbilityComponent::Activate(bool bReset)
{
	Super::Activate(bReset);

	if (!GetOwner()->HasAuthority()) return;
	if (!GetWorld()->IsGameWorld()) return;

	if (bReset)
	{
		DestroyAllAbilities();
		DestroyAllAttributes();
	}

	if (!bAbilitiesInstantiated) InstantiateAbilities();
	if (!bAttributesInstantiated) InstantiateAttributes();

	InitializeAbilities();
	InitializeAttributes();

	OnAbilityComponentReady.Broadcast(this);
}

void ULimenAbilityComponent::Deactivate()
{
	check(GetOwner()->HasAuthority())

	DeactivateAllAbilities();
	DeactivateAllAttributes();

	Super::Deactivate();
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
		if (!Ability.Item.IsValid()) continue;
		Ability->ForceDeactivateAbility();
		Ability->Deinitialize(GetOwner());
	}
}

void ULimenAbilityComponent::DestroyAllAbilities()
{
	check(GetOwner()->HasAuthority())
	for (FAbilityArrayItem& Ability : Abilities.Items)
	{
		if (!Ability.Item.IsValid()) continue;
		Ability->ForceDeactivateAbility();
		Ability->Deinitialize(GetOwner());

		RemoveReplicatedSubObject(*Ability);
		Ability->ConditionalBeginDestroy();
		Ability.Item.Reset();
	}
	Abilities.Items.Empty();
	Abilities.MarkArrayDirty();
	bAbilitiesInstantiated = false;
}

void ULimenAbilityComponent::DeactivateAllAttributes()
{
	check(GetOwner()->HasAuthority())
	for (const FAttributeArrayItem& Attribute : Attributes.Items)
	{
		if (!Attribute.Item.IsValid()) continue;
		Attribute->FreezeAttribute(true);
		Attribute->Deinitialize(GetOwner());
	}
}

void ULimenAbilityComponent::DestroyAllAttributes()
{
	check(GetOwner()->HasAuthority())
	for (FAttributeArrayItem& Attribute : Attributes.Items)
	{
		if (!Attribute.Item.IsValid()) continue;
		Attribute->FreezeAttribute(true);
		Attribute->Deinitialize(GetOwner());

		RemoveReplicatedSubObject(*Attribute);
		Attribute->ConditionalBeginDestroy();
		Attribute.Item.Reset();
	}
	Attributes.Items.Empty();
	Attributes.MarkArrayDirty();
	bAttributesInstantiated = false;
}

ULimenAbilityBase* ULimenAbilityComponent::GetAbility(const TSubclassOf<ULimenAbilityBase> AbilityClass) const
{
	if (AbilityClass)
	{
		for (const FAbilityArrayItem& Ability : Abilities.Items)
		{
			if (*Ability == nullptr) continue;
			if (Ability->IsA(AbilityClass)) return *Ability;
		}
	}

	return nullptr;
}

ULimenAttributeBase* ULimenAbilityComponent::GetAttribute(const TSubclassOf<ULimenAttributeBase> AttributeClass) const
{
	if (AttributeClass)
	{
		for (const FAttributeArrayItem& Attribute : Attributes.Items)
		{
			if (*Attribute == nullptr) continue;
			if (Attribute->IsA(AttributeClass)) return *Attribute;
		}
	}

	return nullptr;
}

TArray<ULimenAbilityBase*> ULimenAbilityComponent::GetAbilities() const
{
	TArray<ULimenAbilityBase*> Out;
	Out.Reserve(Abilities->Num());
	for (const FAbilityArrayItem& Ability : Abilities.Items) Out.Push(*Ability);
	return Out;
}

TArray<ULimenAttributeBase*> ULimenAbilityComponent::GetAttributes() const
{
	TArray<ULimenAttributeBase*> Out;
	Out.Reserve(Attributes->Num());
	for (const FAttributeArrayItem& Attribute : Attributes.Items) Out.Push(*Attribute);
	return Out;
}

void ULimenAbilityComponent::OnRep_ServerAbilityCount()
{
	bAbilitiesLoaded = Abilities->Num() == AuthAbilityCount;
	if (IsNoAuthComponentGameplayReady()) OnAbilityComponentReady.Broadcast(this);
}

void ULimenAbilityComponent::OnRep_ServerAttributeCount()
{
	bAttributesLoaded = Attributes->Num() == AuthAttributeCount;
	if (IsNoAuthComponentGameplayReady()) OnAbilityComponentReady.Broadcast(this);
}

bool ULimenAbilityComponent::IsNoAuthComponentGameplayReady()
{
	check(!GetOwner()->HasAuthority())
	return Abilities->Num() == AuthAbilityCount && Attributes->Num() == AuthAttributeCount;
}

void ULimenAbilityComponent::InitializeAbilities()
{
	for (auto& Ability : Abilities.Items)
	{
		if (Ability.Item.IsValid()) Ability->Initialize(GetOwner());
	}
	bAbilitiesLoaded = true;
}

void ULimenAbilityComponent::InitializeAttributes()
{
	for (auto& Attribute : Attributes.Items)
	{
		if (Attribute.Item.IsValid()) Attribute->Initialize(GetOwner());
	}
	bAttributesLoaded = true;
}

void ULimenAbilityComponent::InstantiateAbilities()
{
	check(GetOwner()->HasAuthority())
	check(Abilities->IsEmpty())

	Abilities->Reserve(AbilityClasses.Num());
	for (auto& AbilityClass : AbilityClasses)
	{
		check(!AbilityClass.IsNull());
		
		ULimenAbilityBase* Ability = NewObject<ULimenAbilityBase>(this, AbilityClass.LoadSynchronous());
		AddReplicatedSubObject(Ability);

		const int32 Index = Abilities->Add(FAbilityArrayItem(Ability));
		Abilities.MarkItemDirty(Abilities[Index]);
	}
	bAbilitiesInstantiated = true;
	AuthAbilityCount = Abilities->Num();
}

void ULimenAbilityComponent::InstantiateAttributes()
{
	check(GetOwner()->HasAuthority())
	check(Attributes->IsEmpty())

	Attributes->Reserve(AttributeClasses.Num());
	for (auto& AttributeClass : AttributeClasses)
	{
		check(!AttributeClass.IsNull());
		
		ULimenAttributeBase* Attribute = NewObject<ULimenAttributeBase>(this,
			AttributeClass.LoadSynchronous());
		AddReplicatedSubObject(Attribute);

		const int32 Index = Attributes->Add(FAttributeArrayItem(Attribute));
		Attributes.MarkItemDirty(Attributes[Index]);
	}
	bAttributesInstantiated = true;
	AuthAttributeCount = Attributes->Num();
}
