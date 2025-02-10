// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Settings/LimenSelectionSetting.h"
#include "Tickable.h"
#include "LimenResolutionSetting.generated.h"


class ULimenGenericModalWidget;
struct FTimerHandle;
struct FScreenResolutionRHI;

/**
 * 
 */
UCLASS()
class LIMENGRAPHICSETTINGS_API ULimenResolutionSetting : public ULimenSelectionSetting, public FTickableGameObject
{
	GENERATED_BODY()

public:
	ULimenResolutionSetting();
	virtual void SubsystemInitialized(ULimenModularSettingsSubsystem* ModularSettingsSubsystem) override;
	virtual bool CanEdit() const override;

#pragma region FTicakbleGameObject

	virtual void Tick( float DeltaTime ) override;
	virtual ETickableTickType GetTickableTickType() const override;
	virtual bool IsTickable() const override;
	virtual TStatId GetStatId() const override;
	virtual bool IsTickableWhenPaused() const override;
	virtual bool IsTickableInEditor() const override;
	virtual UWorld* GetTickableGameObjectWorld() const override;
	
#pragma endregion

	virtual void ApplyCurrentSetting(const bool bUserRequest) override;
	virtual void SetDefaults() override;

private:
	static const FString ConfirmationModalTitle;
	static const FString SecondsTag;
	static const FString ConfirmationModalText;
	static const float ResolutionChangeTimeout;
	
	bool bIsResolutionChangeConfirmed;
	bool bShouldTick;
	float CurrentTime;
	
	FString CurrentWindowModeSetting;
	FString LastWindowModeSetting;

	TWeakObjectPtr<ULimenGenericModalWidget> ModalWidget;
	
	static FString FormatResolution(const FScreenResolutionRHI& InResolution);
	static FString FormatResolution(const uint32 Width, const uint32 Height);
	static FIntPoint DeFormatResolution(const FString& InResolutionString);

	UFUNCTION()
	void OnWindowModeUpdated(const ULimenSetting* Setting);
	UFUNCTION()
	void OnMonitorDisplayUpdated(const ULimenSetting* Setting);

	UFUNCTION()
	void ResolutionConfirmationReceived(ULimenGenericModalWidget* ULimenGenericModalWidget, const bool bConfirmed);

	static FString FormatModalText(const int32 SecondsRemaining);
};
