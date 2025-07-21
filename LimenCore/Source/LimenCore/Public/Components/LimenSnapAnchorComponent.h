// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ArrowComponent.h"
#include "LimenSnapAnchorComponent.generated.h"


class ULimenAnchorPointComponent;


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FActorAchored);

/**
 * @brief The component that attaches to an anchor point
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LIMENCORE_API ULimenSnapAnchorComponent : public UArrowComponent
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	static ULimenSnapAnchorComponent* GetAnchorByTargetId(const AActor* InActor, const FName& TargetId);

	UPROPERTY(BlueprintAssignable)
	FActorAchored OnAnchored;

	ULimenSnapAnchorComponent();

	UFUNCTION(BlueprintCallable)
	virtual void AnchorTo(ULimenAnchorPointComponent* InTarget, bool bAttachToAnchor = true);
	UFUNCTION(BlueprintCallable)
	virtual void AnchorToActor(AActor* InTarget, FName AnchorId, bool bAttachToAnchor = true);
	UFUNCTION(BlueprintCallable)
	virtual void RemoveCurrentAnchor(bool bDetachFromAnchor = true);
	UFUNCTION(BlueprintCallable)
	virtual const FName& GetTargetAnchorId() const;

	ULimenAnchorPointComponent* GetCurrentAnchor() const;

#if WITH_EDITORONLY_DATA

	UFUNCTION(CallInEditor, Category = "Debug", DisplayName="Anchor To Target")
	void DEBUG_AnchorToTarget();
	UFUNCTION(CallInEditor, Category = "Debug", DisplayName="Remove Anchor")
	void DEBUG_RemoveAnchor();

#endif

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anchor")
	FName TargetAnchorId;

#if WITH_EDITORONLY_DATA

	UPROPERTY(EditAnywhere, Category = "Debug")
	TWeakObjectPtr<AActor> AnchorTargetActor;
	UPROPERTY(EditAnywhere, Category = "Debug")
	bool bAttach;

#endif

private:
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	TWeakObjectPtr<ULimenAnchorPointComponent> CurrentAnchor;
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FActorAchoredWithActorPtr, ULimenSnapAnchorComponent*, AnchoredComponent);

/**
 * @brief An anchor point
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ULimenAnchorPointComponent : public UArrowComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FActorAchoredWithActorPtr OnNewAnchoredActor;
	UPROPERTY(BlueprintAssignable)
	FActorAchoredWithActorPtr OnActorAnchorRemoved;

	ULimenAnchorPointComponent();

	UFUNCTION(BlueprintCallable)
	virtual void Anchor(ULimenSnapAnchorComponent* InRequestor, bool bAttachToAnchor = true);
	UFUNCTION(BlueprintCallable)
	virtual void RemoveAnchor(ULimenSnapAnchorComponent* InRequestor, bool bDetachFromAnchor = true);

	const FName& GetAnchorId() const;
	const TArray<ULimenSnapAnchorComponent*>& GetAnchoredComponents() const;

protected:
	UPROPERTY(EditAnywhere, Category = "Anchor")
	FName AnchorId;

private:
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	TArray<ULimenSnapAnchorComponent*> AnchoredComponents;
};
