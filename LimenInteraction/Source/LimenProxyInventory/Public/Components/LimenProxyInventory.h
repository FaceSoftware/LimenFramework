// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LimenProxyInventory.generated.h"


class ALimenItemBase;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LIMENPROXYINVENTORY_API ULimenProxyInventory : public UActorComponent
{
	GENERATED_BODY()

public:
	ULimenProxyInventory();
	virtual void BeginPlay() override;
	void AddItem(const TSubclassOf<ALimenItemBase>& Item);
	void RemoveItem(const int32 Index);

	bool CanAddItem() const;
	TSubclassOf<ALimenItemBase> GetItem(const int32 Index) const;
	int32 GetInventorySize() const;

protected:
	UPROPERTY(EditDefaultsOnly, Category="Limen", meta=(ClampMin="1"))
	int32 InventorySize;

private:
	TArray<TSubclassOf<ALimenItemBase>> Items;
};
