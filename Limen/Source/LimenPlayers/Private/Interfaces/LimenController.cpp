// Copyright Face Software. All Rights Reserved.


#include "Interfaces/LimenController.h"

#include "Kismet/KismetMathLibrary.h"

/*
ILimenController::ILimenController()
{
	if (ILimenController::_getUObject() == nullptr)
	{
		return;
	}
	
	AController* OwnerController = Cast<AController>(ILimenController::_getUObject());
	checkf(OwnerController != nullptr, TEXT("ILimenController should only be used with classes derived from AController"));
	Owner = OwnerController;
}

// Add default functionality here for any ILimenController functions that are not pure virtual.
void ILimenController::LookAt(const FVector& Location)
{
	if (!IsValid(Owner->GetPawn()))
	{
		return;
	}

	const FVector PawnLocation = Owner->GetPawn()->GetActorLocation();
	const FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(PawnLocation, Location);
	Owner->SetControlRotation(LookAtRotation);
	
}
*/
