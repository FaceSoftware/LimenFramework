
// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "LimenPlayerStateBase.generated.h"


UCLASS()
class LIMENGAMEFRAMEWORK_API ALimenPlayerStateBase : public APlayerState
{
	GENERATED_BODY()

public:
	DECLARE_MULTICAST_DELEGATE_OneParam(FLimenPlayerStateDelegate, ALimenPlayerStateBase*)
	FLimenPlayerStateDelegate OnClientReady;
	
	explicit ALimenPlayerStateBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	bool IsLocalPlayerState() const;
	void SetClientReady(bool bIsReady = true);
	bool IsClientReady() const;
	
protected:


private:
	UPROPERTY(ReplicatedUsing=OnRep_ClientReady)
	bool bIsClientReady;
	
	UFUNCTION()
	void OnRep_ClientReady();
};
