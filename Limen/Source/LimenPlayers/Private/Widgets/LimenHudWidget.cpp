// Copyright Face Software. All Rights Reserved.


#include "Widgets/LimenHudWidget.h"


void ULimenHudWidget::BindPawn(APawn* Pawn)
{
	BoundPawn = Pawn;
	OnPawnBound(BoundPawn.Get());
}

APawn* ULimenHudWidget::GetBoundPawn() const
{
	return BoundPawn.Get();
}
