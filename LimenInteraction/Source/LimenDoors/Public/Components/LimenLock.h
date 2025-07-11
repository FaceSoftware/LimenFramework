// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LimenLock.generated.h"


class AController;
class ALimenKey;

UCLASS(BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LIMENDOORS_API ULimenLock : public UActorComponent
{
	GENERATED_BODY()

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FLimenLockEvent, AController*, Controller, APawn*, Pawn, ALimenKey*, key);

	UPROPERTY(BlueprintAssignable)
	FLimenLockEvent OnUnlocked;
	UPROPERTY(BlueprintAssignable)
	FLimenLockEvent OnLocked;
	
	explicit ULimenLock(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void SetKeyword(const FName& NewKeyword);
	UFUNCTION(BlueprintCallable)
	FName GetKeyword() const;
	
	UFUNCTION(BlueprintCallable)
	virtual bool IsLocked() const;
	UFUNCTION(BlueprintCallable)
	virtual void SetLockedState(const bool bLocked);

	UFUNCTION(BlueprintCallable)
	bool ChangeLockedState(AController* Controller, APawn* Pawn, ALimenKey* Test, const bool bLock);
	UFUNCTION(BlueprintCallable)
	bool ChangeLockedStateWithKeys(AController* Controller, APawn* Pawn, TArray<ALimenKey*> Test, const bool bLock);
	UFUNCTION(BlueprintCallable)
	bool TryKey(ALimenKey* Test) const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Limen")
	FName Keyword;

private:
	UPROPERTY(EditAnywhere, Category="Limen")
	bool bStartLocked;
	bool bIsLocked;
};
