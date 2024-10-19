// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interactables/LimenInteractable.h"
#include "Interfaces/LimenSaveObjectInterface.h"
#include "LimenItemBase.generated.h"


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
	UFUNCTION(BlueprintCallable, Category="Limen|Items", BlueprintPure)
	static UTexture2D* GetItemImage(const TSubclassOf<ALimenItemBase>& ItemClass);
	UFUNCTION(BlueprintCallable, Category="Limen|Items", BlueprintPure)
	static FText GetDisplayName(const TSubclassOf<ALimenItemBase>& ItemClass);
	UFUNCTION(BlueprintCallable, Category="Limen|Items", BlueprintPure)
	static FText GetDescription(const TSubclassOf<ALimenItemBase>& ItemClass);
	UFUNCTION(BlueprintCallable, Category="Limen|Items|Actions", BlueprintPure)
	static TArray<ULimenItemAction*> GetItemActions(ULimenInventoryComponent* InventoryComponent, const TSubclassOf<ALimenItemBase>& ItemClass);
	
public:
	explicit ALimenItemBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void BeginPlay() override;

	UTexture2D* GetItemImage() const;
	const FText& GetDisplayName() const;
	const FText& GetDescription() const;

	virtual bool ShouldSaveData() const override;
	virtual bool ShouldLoadData() const override;
	virtual void DataLoaded() override;
	virtual void DataSaved() override;
	
	TArray<ULimenItemAction*> GetItemActions();
	
protected:
	UPROPERTY(EditDefaultsOnly, Category="Limen|Item Actions")
	TArray<TSubclassOf<ULimenItemAction>> ItemActionsClass;
	UPROPERTY()
	TArray<ULimenItemAction*> ItemActions;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Limen")
	FText DisplayName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Limen")
	FText Description;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Limen")
	TSoftObjectPtr<UTexture2D> ItemImage;
	
	virtual void Interact(AController* InController, APawn* InPawn) override;
	virtual void InteractionStopped(AController* InController, APawn* InPawn) override final;
	
private:
	bool bHasBeenLoaded;
};
