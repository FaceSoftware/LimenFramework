// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interactables/LimenInteractable.h"
#include "Interfaces/LimenSaveObjectInterface.h"
#include "UObject/StrongObjectPtr.h"
#include "LimenItemBase.generated.h"


enum EPixelFormat : uint8;
class USpotLightComponent;
class UTextureRenderTarget2D;
class UTexture;
class USceneCaptureComponent2D;
class ULimenInventoryComponent;
class ULimenItemAction;
class ULimenItemDataAsset;
class ALimenItemBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FItemActionRequest, TSubclassOf<ALimenItemBase>, Item, AController*, InController, APawn*, InPawn);

UCLASS(Abstract, Blueprintable, BlueprintType)
class LIMENINTERACTION_API ALimenItemBase : public ALimenInteractable, public ILimenSaveObjectInterface
{
	GENERATED_BODY()

public:
	inline static const FName ItemImageSceneCaptureName = TEXT("ItemImageSceneCapture");

	UFUNCTION(BlueprintCallable, Category="Limen|Items", BlueprintPure, meta=(WorldContext=WorldContextObject))
	static UTexture* GetItemImage(UObject* WorldContextObject, const TSubclassOf<ALimenItemBase>& ItemClass);
	UFUNCTION(BlueprintCallable, Category="Limen|Items", BlueprintPure, meta=(WorldContext=WorldContextObject))
	static FText GetDisplayName(UObject* WorldContextObject, const TSubclassOf<ALimenItemBase>& ItemClass);
	UFUNCTION(BlueprintCallable, Category="Limen|Items", BlueprintPure, meta=(WorldContext=WorldContextObject))
	static FText GetDescription(UObject* WorldContextObject, const TSubclassOf<ALimenItemBase>& ItemClass);
	UFUNCTION(BlueprintCallable, Category="Limen|Items", BlueprintPure, meta=(WorldContext=WorldContextObject))
	static FColor GetRenderTargetBackgroundColor(UObject* WorldContextObject, const TSubclassOf<ALimenItemBase>& ItemClass);

	explicit ALimenItemBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Limen|Items", BlueprintPure)
	UStaticMeshComponent* GetStaticMesh() const;
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Limen|Items", BlueprintPure)
	USkeletalMeshComponent* GetSkeletalMesh() const;
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Limen|Items", BlueprintPure)
	UMeshComponent* GetMesh() const;
	
	UTexture* GetItemImage() const;
	const FText& GetDisplayName() const;
	const FText& GetDescription() const;
	const FColor& GetRenderTargetBackgroundColor() const;

	UFUNCTION(BlueprintCallable)
	void CaptureItemImage();

	virtual bool ShouldSaveData() const override;
	virtual bool ShouldLoadData() const override;
	virtual void PreDataSaved() override;
	virtual void PostDataSaved() override;
	virtual void PreDataLoaded() override;
	virtual void PostDataLoaded() override;
	virtual FName GetUniqueDeterministicId() const override;

	void SetUniqueName(const FName& NewUniqueName);
	
	UFUNCTION(BlueprintCallable, Category="Limen|Items|Actions", BlueprintPure)
	TArray<ULimenItemAction*> GetItemActions() const;
	
	virtual void PickUp(AController* InController, APawn* InPawn);
	virtual void Drop(AController* InController, APawn* InPawn);
	bool IsDropped() const;

	int32 GetItemQuantity() const;
	void SetItemQuantity(const int32 InItemQuantity);

protected:
	UPROPERTY(EditDefaultsOnly, Category="Limen")
	float InteractAnimationTime;
	
	UPROPERTY(EditDefaultsOnly, Category="Limen|Item Actions")
	TArray<TSubclassOf<ULimenItemAction>> ItemActionsClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Item Parameters", meta=(ClampMin="1"))
	int32 ItemQuantity;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Item Parameters")
	FText DisplayName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Item Parameters")
	FText Description;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Item Parameters")
	FName UniqueName;
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

	UFUNCTION(BlueprintImplementableEvent)
	void ItemPickedUp(AController* InController, APawn* InPawn);
	UFUNCTION(BlueprintImplementableEvent)
	void ItemDropped(AController* InController, APawn* InPawn);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic)
	void InteractAnimation(const float AnimationTime);

	UFUNCTION()
	virtual void OnRep_IsDropped();
	
	virtual UStaticMeshComponent* GetStaticMesh_Implementation() const;
	virtual USkeletalMeshComponent* GetSkeletalMesh_Implementation() const;
	virtual UMeshComponent* GetMesh_Implementation() const;
	
private:
	TArray<TStrongObjectPtr<ULimenItemAction>> ItemActions;
	TStrongObjectPtr<UTextureRenderTarget2D> ItemImageRenderTarget2D;
	FTimerHandle InteractAnimationTimerHandle;
	UPROPERTY(ReplicatedUsing=OnRep_IsDropped)
	bool bIsDropped;
};
