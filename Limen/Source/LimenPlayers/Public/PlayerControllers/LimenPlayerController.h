// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PlayerController/LimenPlayerControllerBase.h"
#include "LimenPlayerController.generated.h"


class ALimenPlayerCharacter;
class ULimenItemAction;
class ALimenHUD;
class ALimenItemBase;
enum class EItemAction : uint8;
struct FItemData;
class ULimenGameMenuWrapperWidget;
class ULimenHudWidget;
class ALimenGameplayCharacter;
class ALimenBaseCharacter;
class UInputAction;

UCLASS(Blueprintable)
class LIMENPLAYERS_API ALimenPlayerController : public ALimenPlayerControllerBase
{
	GENERATED_BODY()

public:
	explicit ALimenPlayerController(const FObjectInitializer& InObjectInitializer = FObjectInitializer::Get());
	
	virtual void SetupInputComponent() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

	ALimenPlayerCharacter* GetLimenCharacter() const;

	/**
	 * @brief Do not call this directly. It is used on a multicast delegate.
	 * @param ActionRequested 
	 */
	UFUNCTION()
	void HandleItemActionRequest(ULimenItemAction* ActionRequested);

protected:
	// bool -> true = Toggle visibility
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Limen|Input")
	TSoftObjectPtr<UInputAction> GameMenuInputAction;
	// bool -> true = Toggle pause, false = N/A
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Limen|Input")
	TSoftObjectPtr<UInputAction> PauseMenuInputAction;
	
	TWeakObjectPtr<ALimenHUD> LimenHUD;
	
	/**
	 * @brief Toggles the visibility of the game menu.
	 * @param Instance 
	 */
	void GameMenuInput(const FInputActionInstance& Instance);
	void PauseInput(const FInputActionInstance& Instance);

	virtual bool CreateHudReference() override;

	UFUNCTION()
	virtual void OnPawnDeath(const float Health);

	virtual void BindPawnDelegates(APawn* NewPawn) override;
	virtual void UnbindPawnDelegates(APawn* InPawn) override;

	virtual void BindWidgetDelegates() override;
	virtual void UnbindWidgetDelegates() override;

	
	virtual void LoadingScreenVisibilityChanged(const bool bIsVisible) override;
	UFUNCTION()
	virtual void GameMenuVisibilityChanged(const bool bIsVisible);
	UFUNCTION()
	virtual void PauseMenuVisibilityChanged(const bool bIsVisible);
	UFUNCTION()
	virtual void HUDVisibilityChanged(const bool bIsVisible);
	UFUNCTION()
	virtual void DeathScreenVisibilityChanged(const bool bIsVisible);
	UFUNCTION()
	virtual void MainMenuVisibilityChanged(const bool bIsVisible);

private:
	UPROPERTY()
	TWeakObjectPtr<ALimenPlayerCharacter> LimenPlayerCharacter;
};
