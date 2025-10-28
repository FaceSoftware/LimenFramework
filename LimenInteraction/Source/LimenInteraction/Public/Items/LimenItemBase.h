// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interactables/LimenInteractable.h"
#include "Engine/TextureRenderTarget2D.h"
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
class LIMENINTERACTION_API ALimenItemBase : public ALimenInteractable
{
	GENERATED_BODY()

public:
	inline static const FName ItemImageSceneCaptureName = TEXT("ItemImageSceneCapture");

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
	UTexture* GetItemImage() const;
	UFUNCTION(BlueprintCallable, Category="Limen|Items")
	const FText& GetDisplayName() const;
	UFUNCTION(BlueprintCallable, Category="Limen|Items")
	const FText& GetDescription() const;
	UFUNCTION(BlueprintCallable, Category="Limen|Items")
	const FColor& GetRenderTargetBackgroundColor() const;
	UFUNCTION(BlueprintCallable, Category="Limen|Items")
	TArray<ULimenItemAction*> GetItemActions() const;
	UFUNCTION(BlueprintCallable, Category="Limen|Items")
	bool IsDropped() const;
	UFUNCTION(BlueprintCallable, Category="Limen|Items")
	int32 GetItemQuantity() const;

	UFUNCTION(BlueprintCallable, Category="Limen|Items")
	void CaptureItemImage();
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
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Item Parameters", meta=(EditCondition = "!bUseSceneCaptureForImage"))
	TObjectPtr<UTexture> ItemImage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Item Parameters", meta=(EditCondition = "ItemImageSceneCapture != nullptr"))
	bool bUseSceneCaptureForImage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Item Parameters", meta=(EditCondition = "bUseSceneCaptureForImage"))
	FColor RenderTargetBackgroundColor;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Item Parameters", meta=(EditCondition = "bUseSceneCaptureForImage"))
	FIntPoint RenderTargetResolution;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Item Parameters", meta=(EditCondition = "bUseSceneCaptureForImage"))
	TEnumAsByte<EPixelFormat> RenderTargetPixelFormat;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Item Parameters", meta=(EditCondition = "bUseSceneCaptureForImage"))
	bool bForceRenderTargetLinearGamma;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item Parameters")
	TObjectPtr<USceneCaptureComponent2D> ItemImageSceneCapture;

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
	TStrongObjectPtr<UTextureRenderTarget2D> ItemImageRenderTarget2D;
	FTimerHandle InteractAnimationTimerHandle;

	UFUNCTION()
	void OnRep_PickUpState();
};
