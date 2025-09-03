// Copyright Face Software. All Rights Reserved.


#include "Widgets/LimenObjectiveInventory.h"

#include "Components/LimenObjectiveComponent.h"

void ULimenObjectiveInventory::BindObjectiveComponent(ULimenObjectiveComponent* ObjectiveComponent)
{
	BoundObjectiveComponent = ObjectiveComponent;
	OnObjectiveComponentBound(ObjectiveComponent);
}

ULimenObjectiveComponent* ULimenObjectiveInventory::GetBoundObjectiveComponent() const
{
	return BoundObjectiveComponent.Get();
}
