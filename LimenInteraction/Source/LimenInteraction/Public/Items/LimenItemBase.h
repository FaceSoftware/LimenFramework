// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interactables/LimenInteractable.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Interfaces/LimenInventoryItem.h"
#include "LimenItemBase.generated.h"


enum EPixelFormat : uint8;
class USpotLightComponent;
class UTexture;
class USceneCaptureComponent2D;
class ULimenInventoryComponent;
class ULimenItemAction;
class ULimenItemDataAsset;
class ALimenItemBase;

USTRUCT()
struct FLimenItemBase_PickUpState
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<AController> InController;
	UPROPERTY()
	TObjectPtr<APawn> InPawn;
	UPROPERTY()
	bool bIsDropped = false;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FItemActionRequest, TSubclassOf<ALimenItemBase>, Item, AController*, InController, APawn*, InPawn);

UCLASS(Abstract, Blueprintable, BlueprintType)
class LIMENINTERACTION_API ALimenItemBase : public ALimenInteractable, public ILimenInventoryItem
{
	GENERATED_BODY()

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDropStateChange);
	UPROPERTY(BlueprintAssignable)
	FDropStateChange OnPickedUp;
	UPROPERTY(BlueprintAssignable)
	FDropStateChange OnDropped;

	explicit ALimenItemBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Limen|Items")
	UStaticMeshComponent* GetStaticMesh() const;
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Limen|Items")
	USkeletalMeshComponent* GetSkeletalMesh() const;
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Limen|Items")
	UMeshComponent* GetMesh() const;
	UFUNCTION(BlueprintCallable, Category="Limen|Items")
	bool IsDropped() const;

	// ILimenInventoryItem
	UFUNCTION(BlueprintCallable, Category="Limen|Items")
	virtual UTexture* GetItemImage() const override;
	UFUNCTION(BlueprintCallable, Category="Limen|Items")
	virtual const FText& GetDisplayName() const override;
	UFUNCTION(BlueprintCallable, Category="Limen|Items")
	virtual const FText& GetDescription() const override;
	UFUNCTION(BlueprintCallable, Category="Limen|Items")
	virtual int32 GetItemQuantity() const override;
	UFUNCTION(BlueprintCallable, Category="Limen|Items")
	virtual TArray<ULimenItemAction*> GetItemActions() const override;
	UFUNCTION(BlueprintCallable, Category="Limen|Items")
	virtual AActor* GetActor() override;
	// ILimenInventoryItem

	void PickUp(AController* InController, APawn* InPawn);
	void Drop(AController* InController, APawn* InPawn);
	void SetItemQuantity(const int32 InItemQuantity);

protected:
	UPROPERTY(EditDefaultsOnly, Category="Limen")
	float PickupAnimationTime;
	UPROPERTY(EditDefaultsOnly, Category="Limen|Item Actions")
	TArray<TSubclassOf<ULimenItemAction>> ItemActionsClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Item Parameters", meta=(ClampMin="1"))
	int32 ItemQuantity;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Item Parameters")
	FText DisplayName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Item Parameters")
	FText Description;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Item Parameters")
	TObjectPtr<UTexture> ItemImage;

	virtual void Interact(AController* InController, APawn* InPawn) override final;
	virtual void InteractionStopped(AController* InController, APawn* InPawn) override final;
	virtual void PickedUp(AController* InController, APawn* InPawn);
	virtual void Dropped(AController* InController, APawn* InPawn);
	virtual void PickUpAnimation(const float AnimationTime);
	
	virtual UStaticMeshComponent* GetStaticMesh_Implementation() const;
	virtual USkeletalMeshComponent* GetSkeletalMesh_Implementation() const;
	virtual UMeshComponent* GetMesh_Implementation() const;
	
private:
	UPROPERTY(ReplicatedUsing=OnRep_PickUpState)
	FLimenItemBase_PickUpState PickUpState;
	TArray<TStrongObjectPtr<ULimenItemAction>> ItemActions;
	FTimerHandle InteractAnimationTimerHandle;

	UFUNCTION()
	void OnRep_PickUpState();
};
