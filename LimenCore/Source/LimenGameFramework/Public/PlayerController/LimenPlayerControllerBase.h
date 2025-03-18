// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LimenPlayerControllerBase.generated.h"

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

struct FMouseParameters
{	
	float SensitivityX = 1;
	float SensitivityY = 1;
	bool bInvertAxisX = false;
	bool bInvertAxisY = false;
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
	virtual void SetupInputComponent() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

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
	void SetGameInput();
	UFUNCTION(BlueprintCallable)
	void SetUIInput();
	UFUNCTION(BlueprintCallable)
	void SetUIOnlyInput(const bool bShowMouse = true);
	void SetInputMode(const FInputModeDataBase& InData) override;
	void SetInputMode(const ELimenInputMode InInputMode);
	ELimenInputMode GetInputMode() const;

	UFUNCTION(BlueprintCallable)
	bool CanSeeLocation(const FVector& InLocation) const;
	UFUNCTION(BlueprintCallable)
	bool CanSeeActor(const AActor* OtherActor) const;
	
	const FMouseParameters& GetMouseParameters() const;
	void SetMouseParameters(const FMouseParameters& InNewParams);

protected:
	TWeakObjectPtr<ALimenBaseHUD> LimenBaseHUD;

	virtual void BindPawnDelegates(APawn* NewPawn);
	virtual void UnbindPawnDelegates(APawn* InPawn);
	
	virtual bool CreateHudReference();
	virtual void BindWidgetDelegates();
	virtual void UnbindWidgetDelegates();
	
	UFUNCTION()
	virtual void LoadingScreenVisibilityChanged(const bool bIsVisible);
	
private:
	ELimenInputMode CurrentInputMode;
	
	FMouseParameters MouseParameters;
};
