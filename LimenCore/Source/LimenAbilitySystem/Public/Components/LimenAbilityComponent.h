// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UObject/StrongObjectPtr.h"
#include "LimenAbilityComponent.generated.h"


class ULimenAttributeBase;
class ULimenAbilityBase;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LIMENABILITYSYSTEM_API ULimenAbilityComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	explicit ULimenAbilityComponent(const FObjectInitializer& InObjectInitializer = FObjectInitializer::Get());
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void LoadAbilities(AActor* Owner);
	void LoadAttributes(AActor* Owner);
	void AddAbility(const TSubclassOf<ULimenAbilityBase>& AbilityClass);
	void AddAttribute(const TSubclassOf<ULimenAttributeBase>& AttributeClass);

	bool IsReadyForGameplay() const;

	void DeactivateAllAbilities();

	UFUNCTION(BlueprintCallable, Category="Limen|Abilities", meta=(DeterminesOutputType="AbilityClass"))
	ULimenAbilityBase* GetAbility(const TSubclassOf<ULimenAbilityBase> AbilityClass);

	UFUNCTION(BlueprintCallable, Category="Limen|Abilities", meta=(DeterminesOutputType="AttributeClass"))
	ULimenAttributeBase* GetAttribute(const TSubclassOf<ULimenAttributeBase> AttributeClass);

	template<typename AbilityClass>
	AbilityClass* GetAbility()
	{
		static_assert(std::is_base_of_v<ULimenAbilityBase, AbilityClass>);
		
		for (TStrongObjectPtr<ULimenAbilityBase>& Ability : Abilities)
		{
			AbilityClass* Temp = Cast<AbilityClass>(Ability.Get());
			if (Temp != nullptr)
			{
				return Temp;
			}
		}

		return nullptr;
	}

	template <typename AttributeClass>
	AttributeClass* GetAttribute()
	{
		static_assert(std::is_base_of_v<ULimenAttributeBase, AttributeClass>);
		
		for (TStrongObjectPtr<ULimenAttributeBase>& Attribute : Attributes)
		{
			AttributeClass* Temp = Cast<AttributeClass>(Attribute.Get());
			if (Temp != nullptr)
			{
				return Temp;
			}
		}
		
		return nullptr;
	}

	TArray<ULimenAbilityBase*> GetAbilities() const;
	TArray<ULimenAttributeBase*> GetAttributes() const;
	
	template<typename AbilityClass>
	void InitializeAbilityDependency();

	template<typename AttributeClass>
	void InitializeAttributeDependency();
	
protected:
	UPROPERTY(EditDefaultsOnly, Category="Limen")
	TArray<TSoftClassPtr<ULimenAbilityBase>> AbilityClasses;
	UPROPERTY(EditDefaultsOnly, Category="Limen")
	TArray<TSoftClassPtr<ULimenAttributeBase>> AttributeClasses;

private:
	TArray<TStrongObjectPtr<ULimenAbilityBase>> Abilities;
	
	TArray<TStrongObjectPtr<ULimenAttributeBase>> Attributes;

	bool bAbilitiesLoaded;
	bool bAttributesLoaded;
};
