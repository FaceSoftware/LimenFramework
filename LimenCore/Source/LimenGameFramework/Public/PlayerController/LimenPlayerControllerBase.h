// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LimenPlayerControllerBase.generated.h"


class ULimenMouseSensitivityComponent;
class ALimenBaseHUD;
class ULimenMenuWidget;
class ULimenNotificationComponent;
class UInputAction;
class UInputMappingContext;
class ULimenBaseHudWidget;
class ULimenGameMenuWidget;
class ULimenPauseMenuWidget;
class FLimenNotification;
struct FNotificationParams;
struct FInputActionInstance;

UENUM()
enum class EPauseReason : uint8
{
	Undefined,
	PauseMenu,
	GameMenu,
	Notification,
	Other,
};

enum class ELimenInputMode : uint8
{
	Undefined,
	Game,
	UI,
	UIOnly,
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPauseRequestDelegate, ALimenPlayerControllerBase*, Player, const EPauseReason, PauseReason);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUnPauseRequestDelegate, ALimenPlayerControllerBase*, Player);

UCLASS(Blueprintable)
class LIMENGAMEFRAMEWORK_API ALimenPlayerControllerBase : public APlayerController
{
	GENERATED_BODY()

public:	
	friend FLimenNotification;

	FPauseRequestDelegate OnPauseRequested;
	FUnPauseRequestDelegate OnUnPauseRequested;

	UFUNCTION(BlueprintCallable, BlueprintPure, meta=(WorldContext="Caller"))
	static ALimenPlayerControllerBase* GetLimenPlayerControllerBase(UObject* Caller, int32 PlayerIndex = 0);

	explicit ALimenPlayerControllerBase(const FObjectInitializer& InObjectInitializer = FObjectInitializer::Get());
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category="Limen|Controller")
	void RequestPause(const EPauseReason Reason);
	UFUNCTION(BlueprintCallable, Category="Limen|Controller")
	void RequestUnPause();
	void ToggleRequestPause(const EPauseReason Reason);

	/**
	 * @brief Queues a notification for this player.
	 * @param InParams 
	 */
	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	void QueueNotification(const FNotificationParams& InParams);

	UFUNCTION(BlueprintCallable)
	virtual void SetGameInput();
	UFUNCTION(BlueprintCallable)
	virtual void SetUIInput();
	UFUNCTION(BlueprintCallable)
	void SetUIOnlyInput(const bool bShowMouse = true);
	void SetInputMode(const ELimenInputMode InInputMode);
	ELimenInputMode GetInputMode() const;

	virtual void SetInputMode(const FInputModeDataBase& InData) override;

	UFUNCTION(BlueprintCallable)
	bool CanSeeLocation(const FVector& InLocation) const;
	UFUNCTION(BlueprintCallable)
	bool CanSeeActor(const AActor* OtherActor) const;

	virtual void AddYawInput(const float Val) override;
	virtual void AddPitchInput(const float Val) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<ULimenMouseSensitivityComponent> MouseInputSensitivityComponent;

	TWeakObjectPtr<ALimenBaseHUD> LimenBaseHUD;

	virtual void SetPawn(APawn* InPawn) override;
	virtual void ClientSetHUD_Implementation(TSubclassOf<AHUD> NewHUDClass) override;
	virtual void OnRep_PlayerState() override;
	virtual void SetViewTargetWithBlend(AActor* NewViewTarget, float BlendTime = 0, EViewTargetBlendFunction BlendFunc = VTBlend_Linear, float BlendExp = 0, bool bLockOutgoing = false) override;

	virtual void BindPawnDelegates(APawn* NewPawn);
	virtual void UnbindPawnDelegates(APawn* InPawn);
	virtual void BindWidgetDelegates();
	virtual void UnbindWidgetDelegates();
	virtual void CurrentPawnReady();
	UFUNCTION()
	virtual void LoadingScreenVisibilityChanged(const bool bIsVisible);
	virtual void BlendViewTargetSet(AActor* NewViewTarget);
	
private:
	bool bPawnBeginPlayBound;
	ELimenInputMode CurrentInputMode;

	float MouseYawMultiplier;
	float MousePitchMultiplier;

	FTimerHandle ViewTargetTransitionTimerHandle;

	UFUNCTION()
	virtual void SensitivityUpdated(ULimenMouseSensitivityComponent* Component);

	void CurrentPawnReadyInternal(APawn* InPawn);
};
