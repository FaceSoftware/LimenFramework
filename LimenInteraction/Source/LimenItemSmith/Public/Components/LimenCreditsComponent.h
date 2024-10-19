// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interfaces/LimenSaveObjectInterface.h"
#include "LimenCreditsComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LIMENITEMSMITH_API ULimenCreditsComponent : public UActorComponent
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FCreditsUpdate, const float, CurrentCredits, const float, AmountChanged);

public:
	UPROPERTY(BlueprintAssignable, Category="Limen|Credits")
	FCreditsUpdate OnCreditsCountUpdated;
	
	ULimenCreditsComponent();
	virtual void OnComponentCreated() override;

	UFUNCTION(BlueprintCallable, Category="Limen|Credits")
	void AddCredits(const float Amount);
	
	UFUNCTION(BlueprintCallable, Category="Limen|Credits")
	void RemoveCredits(const float Amount);
	
	UFUNCTION(BlueprintCallable, Category="Limen|Credits")
	float GetCredits() const;

	UFUNCTION(BlueprintCallable, Category="Limen|Credits")
	FString GetCreditsString() const;

	UFUNCTION(BlueprintCallable, Category="Limen|Credits")
	FString GetCreditsName() const;

	UFUNCTION(BlueprintCallable, Category="Limen|Credits")
	bool HasCredits(const float Amount) const;

protected:
	
private:
	UPROPERTY(EditDefaultsOnly)
	FText CreditsName;
	
	UPROPERTY(EditDefaultsOnly)
	float StartingCredits;
	
	UPROPERTY(SaveGame)
	float CurrentCredits;
	
};
