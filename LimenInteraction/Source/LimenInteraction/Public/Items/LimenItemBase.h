// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interactables/LimenInteractable.h"
#include "Interfaces/LimenSaveObjectInterface.h"
#include "UObject/StrongObjectPtr.h"
#include "LimenItemBase.generated.h"


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
	UFUNCTION(BlueprintCallable, Category="Limen|Items", BlueprintPure, meta=(WorldContext=WorldContextObject))
	static UTexture* GetItemImage(UObject* WorldContextObject, const TSubclassOf<ALimenItemBase>& ItemClass);
	UFUNCTION(BlueprintCallable, Category="Limen|Items", BlueprintPure, meta=(WorldContext=WorldContextObject))
	static FText GetDisplayName(UObject* WorldContextObject, const TSubclassOf<ALimenItemBase>& ItemClass);
	UFUNCTION(BlueprintCallable, Category="Limen|Items", BlueprintPure, meta=(WorldContext=WorldContextObject))
	static FText GetDescription(UObject* WorldContextObject, const TSubclassOf<ALimenItemBase>& ItemClass);
	UFUNCTION(BlueprintCallable, Category="Limen|Items", BlueprintPure, meta=(WorldContext=WorldContextObject))
	static FColor GetRenderTargetBackgroundColor(UObject* WorldContextObject, const TSubclassOf<ALimenItemBase>& ItemClass);

	explicit ALimenItemBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Limen|Items", BlueprintPure)
	UStaticMesh* GetItemMesh() const;
	virtual UStaticMesh* GetItemMesh_Implementation() const;
	
	UTexture* GetItemImage() const;
	const FText& GetDisplayName() const;
	const FText& GetDescription() const;
	const FColor& GetRenderTargetBackgroundColor() const;

	virtual bool ShouldSaveData() const override;
	virtual bool ShouldLoadData() const override;
	virtual void DataLoaded() override;
	virtual void DataSaved() override;
	
	UFUNCTION(BlueprintCallable, Category="Limen|Items|Actions", BlueprintPure)
	TArray<ULimenItemAction*> GetItemActions() const;
	
protected:
	UPROPERTY(EditDefaultsOnly, Category="Limen|Item Actions")
	TArray<TSubclassOf<ULimenItemAction>> ItemActionsClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Limen")
	FText DisplayName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Limen")
	FText Description;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Limen", meta=(EditCondition = "!bUseSceneCaptureForImage"))
	TObjectPtr<UTexture> ItemImage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Limen", meta=(EditCondition = "ItemImageSceneCapture != nullptr"))
	bool bUseSceneCaptureForImage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Limen", meta=(EditCondition = "bUseSceneCaptureForImage"))
	FColor RenderTargetBackgroundColor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Limen")
	TObjectPtr<USceneCaptureComponent2D> ItemImageSceneCapture;
	
	virtual void Interact(AController* InController, APawn* InPawn) override;
	virtual void InteractionStopped(AController* InController, APawn* InPawn) override final;
	
private:
	TArray<TStrongObjectPtr<ULimenItemAction>> ItemActions;
	bool bHasBeenLoaded;

	UPROPERTY()
	TObjectPtr<UTextureRenderTarget2D> ItemImageRenderTarget2D;
};
