// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interactables/LimenInteractable.h"
#include "LimenDoorBase.generated.h"


class ALimenKey;

UCLASS(Abstract, Blueprintable, BlueprintType)
class LIMENDOORS_API ALimenDoorBase : public ALimenInteractable
{
	GENERATED_BODY()

public:
	explicit ALimenDoorBase(const FObjectInitializer& InObjectInitializer = FObjectInitializer::Get());
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category="Limen|Doors")
	void SetStartOpen(const bool bNewStartOpen);
	
	UFUNCTION(BlueprintCallable, Category="Limen|Doors")
	void OpenDoor(AController* InController = nullptr, APawn* InPawn = nullptr, const bool bIgnoreRestrictions = true);
	UFUNCTION(BlueprintCallable, Category="Limen|Doors")
	void OpenDoorWithoutAnimation(AController* InController = nullptr, APawn* InPawn = nullptr, const bool bIgnoreRestrictions = true);

	UFUNCTION(BlueprintCallable, Category="Limen|Doors")
	void CloseDoor(AController* InController = nullptr, APawn* InPawn = nullptr);
	UFUNCTION(BlueprintCallable, Category="Limen|Doors")
	void CloseDoorWithoutAnimation(AController* InController = nullptr, APawn* InPawn = nullptr);
	
	UFUNCTION(BlueprintCallable, Category="Limen|Doors")
	bool IsOpen() const;

protected:
	UPROPERTY(EditAnywhere, Category="Limen|Config")
	bool bStartOpen;
	UPROPERTY(EditAnywhere)
	bool bUseAnimationOnStart;
	UPROPERTY(EditAnywhere, Category="Limen")
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

	void DoorStateChanged(const bool bDoorIsOpen);
	
private:
	bool bIsOpen;
};
