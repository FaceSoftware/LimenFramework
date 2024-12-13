// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LimenPlayerCharacter.h"
#include "LimenFirstPersonCharacter.generated.h"


class ULimenCameraShakeComponent;
class ULimenLineTraceInteractionComponent;

UCLASS(Blueprintable, BlueprintType)
class LIMENPLAYERS_API ALimenFirstPersonCharacter : public ALimenPlayerCharacter
{
	GENERATED_BODY()

public:
	static const FName InteractionComponentName;
	static const FName CameraShakeComponentName;
	
	explicit ALimenFirstPersonCharacter(const FObjectInitializer& InObjectInitializer = FObjectInitializer::Get());
	virtual void BeginPlay() override;

	virtual void Interact() override;
	virtual void StopInteraction() override;
	virtual ULimenInteractionComponent* GetInteractionComponent() const override;
	
protected:
	virtual void LookInput(const FInputActionInstance& Instance) override;
	
private:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<ULimenLineTraceInteractionComponent> CharacterInteraction;
};
