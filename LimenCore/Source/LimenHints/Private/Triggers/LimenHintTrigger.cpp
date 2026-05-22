// Copyright © FaceSoftware. All rights reserved.


#include "Triggers/LimenHintTrigger.h"

#include "Objects/LimenHint.h"


void ALimenHintTrigger::BeginPlay()
{
	Super::BeginPlay();

	HintInstance = TStrongObjectPtr(NewObject<ULimenHint>(this, HintClass.LoadSynchronous()));
	if (HintInstance.IsValid())
	{
		HintInstance->SetHintWidgetClass(HintWidgetClass.LoadSynchronous());
		HintInstance->Initialize();
	}
}

void ALimenHintTrigger::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	HintInstance.Reset();
}

void ALimenHintTrigger::ComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                              UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Super::ComponentBeginOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	if (const APawn* Pawn = Cast<APawn>(OtherActor); !Pawn || !Pawn->IsPlayerControlled()) { return; }

	if (HintInstance.IsValid())
	{
		HintInstance->Enable();
	}
}
