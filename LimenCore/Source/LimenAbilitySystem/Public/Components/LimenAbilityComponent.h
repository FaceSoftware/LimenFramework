// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/LimenAbilityBase.h"
#include "Attributes/LimenAttributeBase.h"
#include "Components/ActorComponent.h"
#include "GameFramework/Pawn.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "Network/LimenNetworkUtils.h"
#include "LimenAbilityComponent.generated.h"


struct FAbilityArray;
USTRUCT()
struct FAbilityArrayItem : public FFastArraySerializerItem
{
	GENERATED_BODY()

	FAbilityArrayItem() = default;
	explicit FAbilityArrayItem(ULimenAbilityBase* Ability)
	{
		Item = TStrongObjectPtr(Ability);
	}

	TStrongObjectPtr<ULimenAbilityBase> Item;

	FAbilityArrayItem& operator=(ULimenAbilityBase* Ability)
	{
		Item = TStrongObjectPtr(Ability);
		return *this;
	}
	ULimenAbilityBase* operator->()
	{
		return Item.Get();
	}
	ULimenAbilityBase* operator->() const
	{
		return Item.Get();
	}
	ULimenAbilityBase* operator*()
	{
		return Item.Get();
	}
	ULimenAbilityBase* operator*() const
	{
		return Item.Get();
	}

	void PreReplicatedRemove(const FAbilityArray& InArraySerializer) {}
	void PostReplicatedAdd(const FAbilityArray& InArraySerializer) {}
	void PostReplicatedChange(const FAbilityArray& InArraySerializer) {}
	FString GetDebugString() { return TEXT(""); }
};
USTRUCT()
struct FAbilityArray : public FFastArraySerializer
{
	GENERATED_BODY()

	UPROPERTY(Transient)
	TArray<FAbilityArrayItem> Items;

	TArray<FAbilityArrayItem>* operator->()
	{
		return &Items;
	}
	const TArray<FAbilityArrayItem>* operator->() const
	{
		return &Items;
	}
	TArray<FAbilityArrayItem>* operator*()
	{
		return &Items;
	}
	const TArray<FAbilityArrayItem>* operator*() const
	{
		return &Items;
	}
	FAbilityArrayItem& operator[](const int32 Index)
	{
		check(Items.IsValidIndex(Index));
		return Items[Index];
	}
	const FAbilityArrayItem& operator[](const int32 Index) const
	{
		check(Items.IsValidIndex(Index));
		return Items[Index];
	}

	operator TArray<FAbilityArrayItem>&()
	{
		return Items;
	}
	operator const TArray<FAbilityArrayItem>&()
	{
		return Items;
	}

	bool NetDeltaSerialize(FNetDeltaSerializeInfo & DeltaParms)
	{
		return FastArrayDeltaSerialize<FAbilityArrayItem, FAbilityArray>( Items, DeltaParms, *this );
	}
};
DECLARE_STRUCT_OPS_TYPE_TRAITS(FAbilityArray)

struct FAttributeArray;
USTRUCT()
struct FAttributeArrayItem : public FFastArraySerializerItem
{
	GENERATED_BODY()

	FAttributeArrayItem() = default;
	explicit FAttributeArrayItem(ULimenAttributeBase* Attribute)
	{
		Item = TStrongObjectPtr(Attribute);
	}

	TStrongObjectPtr<ULimenAttributeBase> Item;

	FAttributeArrayItem& operator=(ULimenAttributeBase* Attribute)
	{
		Item = TStrongObjectPtr(Attribute);
		return *this;
	}
	bool operator==(const FAttributeArrayItem& Other) const
	{
		return Item.Get() == Other.Item.Get();
	}
	ULimenAttributeBase* operator->() const
	{
		return Item.Get();
	}
	ULimenAttributeBase* operator->()
	{
		return Item.Get();
	}
	ULimenAttributeBase* operator*()
	{
		return Item.Get();
	}
	ULimenAttributeBase* operator*() const
	{
		return Item.Get();
	}

	void PreReplicatedRemove(const FAttributeArray& InArraySerializer) {}
	void PostReplicatedAdd(const FAttributeArray& InArraySerializer) {}
	void PostReplicatedChange(const FAttributeArray& InArraySerializer) {}
	FString GetDebugString() { return TEXT(""); }
};
USTRUCT()
struct FAttributeArray : public FFastArraySerializer
{
	GENERATED_BODY()

	UPROPERTY(Transient)
	TArray<FAttributeArrayItem> Items;

	TArray<FAttributeArrayItem>* operator->()
	{
		return &Items;
	}
	const TArray<FAttributeArrayItem>* operator->() const
	{
		return &Items;
	}
	TArray<FAttributeArrayItem>* operator*()
	{
		return &Items;
	}
	const TArray<FAttributeArrayItem>* operator*() const
	{
		return &Items;
	}
	FAttributeArrayItem& operator[](const int32 Index)
	{
		check(Items.IsValidIndex(Index));
		return Items[Index];
	}
	const FAttributeArrayItem& operator[](const int32 Index) const
	{
		check(Items.IsValidIndex(Index));
		return Items[Index];
	}

	operator TArray<FAttributeArrayItem>&()
	{
		return Items;
	}
	operator const TArray<FAttributeArrayItem>&()
	{
		return Items;
	}

	bool NetDeltaSerialize(FNetDeltaSerializeInfo & DeltaParms)
	{
		return FastArrayDeltaSerialize<FAttributeArrayItem, FAttributeArray>( Items, DeltaParms, *this );
	}
};
DECLARE_STRUCT_OPS_TYPE_TRAITS(FAttributeArray)

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAbilityComponentReady, ULimenAbilityComponent*, AbilityComponent);

/**
 * The ULimenAbilityComponent class is a component designed to manage, load,
 * and interact with abilities and attributes within the Limen ability system framework.
 * This component provides functionality to handle gameplay-related capabilities,
 * including ability initialization, attribute management, and dependency resolution.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LIMENABILITYSYSTEM_API ULimenAbilityComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	/**
	 * @brief Signals that all abilities and attributes are instantiated.
	 * Useful for clients since the objects are spawned over the network and will have a delay.
	 * On authority, this is executed after Activate().
	 */
	UPROPERTY(BlueprintAssignable, Category="Limen|Ability Component")
	FAbilityComponentReady OnAbilityComponentReady;

	explicit ULimenAbilityComponent(const FObjectInitializer& InObjectInitializer = FObjectInitializer::Get());
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Activate(bool bReset = false) override;
	virtual void Deactivate() override;
	
	/**
	 * Adds a new ability to the component and initializes it.
	 *
	 * @param AbilityClass The class type of the ability being added. This defines the behavior and properties of the new ability.
	 */
	void AddAbility(const TSubclassOf<ULimenAbilityBase>& AbilityClass);
	/**
	 * Adds a new attribute to the component and initializes it.
	 *
	 * @param AttributeClass The class type of the attribute being added. This defines the properties and behavior of the new attribute.
	 */
	void AddAttribute(const TSubclassOf<ULimenAttributeBase>& AttributeClass);

	/**
	 * Determines whether the ability component is ready for gameplay.
	 *
	 * @return True if abilities and attributes have been successfully loaded and initialized; false otherwise.
	 */
	UFUNCTION(BlueprintCallable, Category="Limen|Ability Component")
	bool IsReadyForGameplay() const;
	/**
	 * Deactivates all abilities associated with the component.
	 *
	 * Iterates through the list of abilities and forces their deactivation. This ensures
	 * that no active abilities persist, which can be useful in scenarios like player death
	 * or transitioning out of gameplay.
	 */
	void DeactivateAllAbilities();
	void DestroyAllAbilities();
	/**
	 * Deactivates all abilities associated with the component.
	 *
	 * Iterates through the list of abilities and forces their deactivation. This ensures
	 * that no active abilities persist, which can be useful in scenarios like player death
	 * or transitioning out of gameplay.
	 */
	void DeactivateAllAttributes();
	void DestroyAllAttributes();
	/**
	 * Retrieves an ability of the specified class type from the component's managed abilities.
	 * This method searches for an ability that matches the provided class or any subclass of the specified class within the component's ability collection.
	 *
	 * @param AbilityClass The class type of the ability to search for.
	 * @return A pointer to the matching ability if found; otherwise, returns nullptr.
	 */
	UFUNCTION(BlueprintCallable, Category="Limen|Abilities", meta=(DeterminesOutputType="AbilityClass"))
	ULimenAbilityBase* GetAbility(const TSubclassOf<ULimenAbilityBase> AbilityClass) const;
	/**
	 * Retrieves an attribute of the specified class type from the managed attributes.
	 * This method searches for an attribute that matches the provided class or any
	 * subclass of the specified class within the component's attribute collection.
	 *
	 * @param AttributeClass The class type of the attribute to search for.
	 * @return A pointer to the matching attribute if found, otherwise nullptr.
	 */
	UFUNCTION(BlueprintCallable, Category="Limen|Abilities", meta=(DeterminesOutputType="AttributeClass"))
	ULimenAttributeBase* GetAttribute(const TSubclassOf<ULimenAttributeBase> AttributeClass) const;
	/**
	 * Retrieves an ability of the specified type managed by the component.
	 *
	 * This method iterates through the collection of abilities stored in the component
	 * and searches for one that matches the specified type or a subclass of that type.
	 * If a matching ability is found, it is returned; otherwise, the method will return nullptr.
	 *
	 * @tparam AbilityClass The type of the ability being searched for. Must inherit from ULimenAbilityBase.
	 * @return A pointer to the matching ability if found; otherwise, nullptr.
	 */
	template<typename AbilityClass = ULimenAbilityBase>
	AbilityClass* GetAbility();
	/**
	 * Retrieves an attribute of the specified type managed by the component.
	 *
	 * This method iterates through the collection of attributes stored in the component
	 * and searches for one that matches the specified type or a subclass of that type.
	 * If a matching attribute is found, it is returned; otherwise, the method will return nullptr.
	 *
	 * @tparam AttributeClass The type of the attribute being searched for. Must inherit from ULimenAttributeBase.
	 * @return A pointer to the matching attribute if found; otherwise, nullptr.
	 */
	template<typename AttributeClass = ULimenAttributeBase>
	AttributeClass* GetAttribute() const;
	/**S 
	 * Retrieves a list of abilities currently associated with the component.
	 *
	 * @return A TArray containing pointers to ULimenAbilityBase objects
	 * representing the abilities managed by this component.
	 */
	TArray<ULimenAbilityBase*> GetAbilities() const;
	/**
	 * Retrieves all attributes currently associated with this ability component.
	 *
	 * @return An array of pointers to ULimenAttributeBase representing the attributes of this component.
	 */
	TArray<ULimenAttributeBase*> GetAttributes() const;
	/**
	 * Ensures that the specified ability dependency is initialized. This method checks if
	 * the ability associated with the given ability class is present and uninitialized,
	 * and if so, initializes it using the owner of the component.
	 */
	template<typename AbilityClass = ULimenAbilityBase>
	void InitializeAbilityDependency();
	/**
	 * Ensures that an attribute dependency within the ability system is properly initialized.
	 * If the specified attribute is not already initialized, this method will invoke its initialization
	 * process using the owning pawn, ensuring the attribute is ready for use.
	 */
	template<typename AttributeClass = ULimenAttributeBase>
	void InitializeAttributeDependency();
	
protected:
	/**
	 * Array of soft class pointers referencing ability classes in the Limen system.
	 * These classes define the abilities that can be associated with the actor and
	 * are loaded and used dynamically during gameplay.
	 */
	UPROPERTY(EditDefaultsOnly, Category="Limen")
	TArray<TSoftClassPtr<ULimenAbilityBase>> AbilityClasses;
	/**
	 * Array of attribute class references used to define and manage specific
	 * attributes within the Limen ability system. These classes serve as templates
	 * for attributes associated with gameplay elements.
	 */
	UPROPERTY(EditDefaultsOnly, Category="Limen")
	TArray<TSoftClassPtr<ULimenAttributeBase>> AttributeClasses;

private:
	UPROPERTY(Replicated, Transient)
	FAbilityArray Abilities;
	UPROPERTY(Replicated, Transient)
	FAttributeArray Attributes;
	UPROPERTY(ReplicatedUsing=OnRep_ServerAbilityCount, Transient)
	int32 AuthAbilityCount;
	UPROPERTY(ReplicatedUsing=OnRep_ServerAttributeCount, Transient)
	int32 AuthAttributeCount;

	bool bAbilitiesLoaded;
	bool bAttributesLoaded;
	bool bAbilitiesInstantiated;
	bool bAttributesInstantiated;

	UFUNCTION()
	void OnRep_ServerAbilityCount();
	UFUNCTION()
	void OnRep_ServerAttributeCount();

	bool IsNoAuthComponentGameplayReady();

	void InitializeAbilities();
	void InitializeAttributes();
	void InstantiateAbilities();
	void InstantiateAttributes();
};


template <typename AbilityClass>
AbilityClass* ULimenAbilityComponent::GetAbility()
{
	static_assert(std::is_base_of_v<ULimenAbilityBase, AbilityClass>);
	return CastChecked<AbilityClass>(GetAbility(AbilityClass::StaticClass()), ECastCheckedType::NullAllowed);
}

template <typename AttributeClass>
AttributeClass* ULimenAbilityComponent::GetAttribute() const
{
	static_assert(std::is_base_of_v<ULimenAttributeBase, AttributeClass>);
	return CastChecked<AttributeClass>(GetAttribute(AttributeClass::StaticClass()), ECastCheckedType::NullAllowed);
}

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
