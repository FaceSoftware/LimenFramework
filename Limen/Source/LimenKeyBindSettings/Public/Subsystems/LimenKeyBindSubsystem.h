// Copyright © 2024 FaceSoftware. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/LimenModularSettingsSubsystem.h"
#include "LimenKeyBindSubsystem.generated.h"


class UInputMappingContext;
class ULimenKeyBindDeveloperSettings;

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
	ULimenKeyBindSubsystem();
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void LoadDefaultSettingsList() override;

	template<typename T>
	UInputMappingContext* GetPlayerInputMappingContext() const;
	template<typename T>
	UInputMappingContext* GetPawnInputMappingContext() const;
	UInputMappingContext* GetPlayerInputMappingContext(const APlayerController* PlayerController) const;
	UInputMappingContext* GetPawnInputMappingContext(const APawn* Pawn) const;
	UInputMappingContext* GetPlayerInputMappingContext(const TSubclassOf<APlayerController>& PlayerController) const;
	UInputMappingContext* GetPawnInputMappingContext(const TSubclassOf<APawn>& Pawn) const;

private:
	UPROPERTY()
	TObjectPtr<const ULimenKeyBindDeveloperSettings> SubsystemSettings;

	UPROPERTY(EditDefaultsOnly)
	TMap<TSoftClassPtr<APlayerController>, TSoftObjectPtr<UInputMappingContext>> PlayerMappingContexts;
	UPROPERTY(EditDefaultsOnly)
	TMap<TSoftClassPtr<APawn>, TSoftObjectPtr<UInputMappingContext>> PawnMappingContexts;
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
