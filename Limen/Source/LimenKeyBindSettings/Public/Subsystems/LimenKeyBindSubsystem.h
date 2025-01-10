// Copyright © 2024 FaceSoftware. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedActionKeyMapping.h"
#include "Subsystems/LimenModularSettingsSubsystem.h"
#include "UObject/StrongObjectPtr.h"
#include "LimenKeyBindSubsystem.generated.h"


class UInputMappingContext;
class ULimenKeyBindDeveloperSettings;

DECLARE_MULTICAST_DELEGATE(FKeyBindUpdate)

/**
 * 
 */
UCLASS()
class LIMENKEYBINDSETTINGS_API ULimenKeyBindSubsystem : public ULimenModularSettingsSubsystem
{
	GENERATED_BODY()
	
	using FMappingContextSoftPtr = TSoftObjectPtr<UInputMappingContext>;
	using FPawnClassSoftPtr = TSoftClassPtr<APawn>;
	using FPlayerControllerClassSoftPtr = TSoftClassPtr<APlayerController>;

public:
	FKeyBindUpdate OnKeyBindUpdate;
	
	ULimenKeyBindSubsystem();
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void LoadDefaultSettingsList() override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	template<typename T>
	UInputMappingContext* GetPlayerInputMappingContext() const;
	template<typename T>
	UInputMappingContext* GetPawnInputMappingContext() const;
	UInputMappingContext* GetPlayerInputMappingContext(const APlayerController* PlayerController) const;
	UInputMappingContext* GetPawnInputMappingContext(const APawn* Pawn) const;
	UInputMappingContext* GetPlayerInputMappingContext(const TSubclassOf<APlayerController>& PlayerController) const;
	UInputMappingContext* GetPawnInputMappingContext(const TSubclassOf<APawn>& Pawn) const;

	UInputMappingContext* GetInputMappingByAction(const UInputAction* Action) const;
	FEnhancedActionKeyMapping& GetActionKeyMappingByAction(const UInputAction* Action);
	bool SetActionKeyMappingByAction(const UInputAction* Action, const FEnhancedActionKeyMapping& InActionKeyMapping);

private:
	UPROPERTY()
	TObjectPtr<const ULimenKeyBindDeveloperSettings> SubsystemSettings;
	
	TMap<TSoftClassPtr<APlayerController>, UInputMappingContext*> PlayerMappingContexts;
	TMap<TSoftClassPtr<APawn>, UInputMappingContext*> PawnMappingContexts;
	
	TArray<TStrongObjectPtr<UInputMappingContext>> MappingContexts;

	UFUNCTION()
	void SettingApplied(const ULimenSetting* Setting);
};

template <typename T>
UInputMappingContext* ULimenKeyBindSubsystem::GetPlayerInputMappingContext() const
{
	static_assert(TIsDerivedFrom<T, APlayerController>::Value);

	return GetPlayerInputMappingContext(T::StaticClass());
}

template <typename T>
UInputMappingContext* ULimenKeyBindSubsystem::GetPawnInputMappingContext() const
{
	static_assert(TIsDerivedFrom<T, APawn>::Value);

	return GetPawnInputMappingContext(T::StaticClass());
}
