// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LimenAbilityComponent.generated.h"


class ULimenAttributeBase;
class ULimenAbilityBase;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LIMENABILITYSYSTEM_API ULimenAbilityComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	explicit ULimenAbilityComponent(const FObjectInitializer& InObjectInitializer = FObjectInitializer::Get());
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void LoadAbilities();
	void LoadAttributes();
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
		
		for (ULimenAbilityBase* Ability : Abilities)
		{
			AbilityClass* Temp = dynamic_cast<AbilityClass*>(Ability);
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
		
		for (ULimenAttributeBase* Attribute : Attributes)
		{
			AttributeClass* Temp = dynamic_cast<AttributeClass*>(Attribute);
			if (Temp != nullptr)
			{
				return Temp;
			}
		}
		
		return nullptr;
	}

	const TArray<ULimenAbilityBase*>& GetAbilities();
	const TArray<ULimenAttributeBase*>& GetAttributes();
	
	template<typename AbilityClass>
	void InitializeAbilityDependency();

	template<typename AttributeClass>
	void InitializeAttributeDependency();
	
protected:

private:
	UPROPERTY(EditDefaultsOnly, Category="Limen")
	TArray<TSoftClassPtr<ULimenAbilityBase>> AbilityClasses;
	UPROPERTY()
	TArray<TObjectPtr<ULimenAbilityBase>> Abilities;
	
	UPROPERTY(EditDefaultsOnly, Category="Limen")
	TArray<TSoftClassPtr<ULimenAttributeBase>> AttributeClasses;
	UPROPERTY()
	TArray<TObjectPtr<ULimenAttributeBase>> Attributes;

	bool bAbilitiesLoaded;
	bool bAttributesLoaded;
};
