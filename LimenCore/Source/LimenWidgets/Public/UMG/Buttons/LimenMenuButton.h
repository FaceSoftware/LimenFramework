// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LimenStandardButton.h"
#include "Tickable.h"
#include "LimenMenuButton.generated.h"


class SButton;
class SImage;
class SWidget;
class SRetainerWidget;

/**
 * 
 */
UCLASS()
class LIMENWIDGETS_API ULimenMenuButton : public ULimenStandardButton, public FTickableGameObject
{
	GENERATED_BODY()

public:
	ULimenMenuButton();
	virtual void PostInitProperties() override;
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
	
#pragma region FTickableGameObject

	virtual void Tick(float DeltaTime) override;
	virtual ETickableTickType GetTickableTickType() const override;
	virtual bool IsTickable() const override;
	virtual TStatId GetStatId() const override;
	
	virtual bool IsTickableWhenPaused() const override;
	virtual bool IsTickableInEditor() const override;
	virtual UWorld* GetTickableGameObjectWorld() const override;

#pragma endregion FTickableGameObject

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Button|Icon")
	bool bUseIcon;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Button|Icon", meta=(EditCondition="bUseIcon"))
	FSlateBrush IconBrush;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Button|Icon", meta=(EditCondition="bUseIcon"))
	FMargin IconMargin;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Button|Effects")
	FName TextureParameter;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Button|Effects")
	FName ToggleParameter;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Button|Effects")
	TObjectPtr<UMaterialInterface> EffectMaterial;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Button|Effects")
	float EffectDurationSeconds;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Button|Effects|Hover")
	TObjectPtr<USoundBase> HoverSound;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Button|Effects|Unhover")
	TObjectPtr<USoundBase> UnhoverSound;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Button|Effects|Click")
	TObjectPtr<USoundBase> ClickSound;

	TSharedPtr<SButton> ButtonWidget;
	
	virtual TSharedRef<SWidget> RebuildWidget() override;

	virtual FReply OnClicked() override;
	virtual void OnHovered() override;
	virtual void OnUnhovered() override;

	TSharedRef<SImage> GetIcon() const;

	bool PlayDistortionEffect();
	UFUNCTION()
	void StopDistortionEffect();
	
private:
	TSharedPtr<SImage> Icon;
	TSharedPtr<SRetainerWidget> RetainerBox;

	FTimerHandle HoverEffectTimer;

	bool bIsPlayingDistortionEffect;
	float HoverEffectElapsedTime;
};
