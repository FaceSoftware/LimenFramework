// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LimenKeyLock.generated.h"


class ALimenKey;

UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LIMENDOORS_API ULimenKeyLock : public UActorComponent
{
	GENERATED_BODY()

public:
	ULimenKeyLock();
	virtual void OnComponentCreated() override;
	virtual void BeginPlay() override;
	
	bool Unlock(const ALimenKey* TestKey);
	bool IsLocked() const;

protected:
	UFUNCTION(BlueprintCallable)
	void Lock();
	UFUNCTION(BlueprintCallable)
	void Unlock();
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	ALimenKey* SetKey();

private:
	UPROPERTY(EditAnywhere, Category="Limen|Config")
	bool bIsLocked;
	UPROPERTY(EditAnywhere, Category="Limen|Config", meta=(EditCondition="bIsLocked"))
	bool bUsesKey;
	TObjectPtr<ALimenKey> Key;
};
