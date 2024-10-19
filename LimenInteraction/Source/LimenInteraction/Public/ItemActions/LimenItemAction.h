// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "LimenItemAction.generated.h"

class ALimenItemBase;
/**
 * 
 */
UCLASS(Abstract, BlueprintType)
class LIMENINTERACTION_API ULimenItemAction : public UObject
{
	GENERATED_BODY()

public:
	ULimenItemAction();
	
	virtual void SetupAction(ALimenItemBase* InItem);
	UFUNCTION(BlueprintCallable, Category="Limen|Items")
	virtual void ActivateAction();

	
	UFUNCTION(BlueprintCallable, Category="Limen|Item Actions")
	virtual bool CanPerformAction() const;
	UFUNCTION(BlueprintCallable, Category="Limen|Item Actions")
	const FText& GetDisplayName() const;
	UFUNCTION(BlueprintCallable, Category="Limen|Item Actions")
	const FText& GetDescription() const;
	UFUNCTION(BlueprintCallable, Category="Limen|Item Actions")
	ALimenItemBase* GetBoundItem() const;

protected:
	FText ActionDisplayName;
	FText ActionDescription;

private:
	UPROPERTY()
	TObjectPtr<ALimenItemBase> BoundItem;
};
