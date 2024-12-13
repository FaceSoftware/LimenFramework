// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interactables/LimenInteractable.h"
#include "LimenDoorBase.generated.h"


class ULimenLock;
class ALimenKey;

enum class EDoorState : uint8
{
	Undefined,
	Open,
	Closed,
	Opening,
	Closing,
};

UCLASS(Abstract, Blueprintable, BlueprintType)
class LIMENDOORS_API ALimenDoorBase : public ALimenInteractable
{
	GENERATED_BODY()

public:
	DECLARE_MULTICAST_DELEGATE_OneParam(FOpenStateChangedDelegate, EDoorState /* DoorState */);
	
	FOpenStateChangedDelegate OnDoorStateChanged;
	
	explicit ALimenDoorBase(const FObjectInitializer& InObjectInitializer = FObjectInitializer::Get());
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category="Limen|Doors")
	void SetStartOpen(const bool bNewStartOpen);
	
	UFUNCTION(BlueprintCallable, Category="Limen|Doors")
	virtual void OpenDoor(AController* InController = nullptr, APawn* InPawn = nullptr, const bool bIgnoreRestrictions = true);
	UFUNCTION(BlueprintCallable, Category="Limen|Doors")
	virtual void OpenDoorWithoutAnimation(AController* InController = nullptr, APawn* InPawn = nullptr, const bool bIgnoreRestrictions = true);

	UFUNCTION(BlueprintCallable, Category="Limen|Doors")
	virtual void CloseDoor(AController* InController = nullptr, APawn* InPawn = nullptr);
	UFUNCTION(BlueprintCallable, Category="Limen|Doors")
	virtual void CloseDoorWithoutAnimation(AController* InController = nullptr, APawn* InPawn = nullptr);
	
	UFUNCTION(BlueprintCallable, Category="Limen|Doors")
	bool IsOpen() const;

	EDoorState GetDoorState() const;

protected:
	UPROPERTY(EditAnywhere, Category="Limen|Config")
	bool bStartOpen;
	UPROPERTY(EditDefaultsOnly, Category="Limen")
	FName DoorComponentTag;
	
	virtual void Interact(AController* InController, APawn* InPawn) override;

	UFUNCTION(BlueprintImplementableEvent, Category="Limen|Doors")
	void DoorOpenAnimation();
	UFUNCTION(BlueprintImplementableEvent, Category="Limen|Doors")
	void DoorCloseAnimation();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="Limen|Doors")
	void SetDoorOpenWithoutAnimation();
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="Limen|Doors")
	void SetDoorCloseWithoutAnimation();

	void ToggleOpenState();

	UFUNCTION(BlueprintCallable)
	void NotifyAnimationFinished(const bool bIsOpenAnimation);

	void SetDoorState(const EDoorState NewState);
	
private:
	EDoorState DoorState;
};
