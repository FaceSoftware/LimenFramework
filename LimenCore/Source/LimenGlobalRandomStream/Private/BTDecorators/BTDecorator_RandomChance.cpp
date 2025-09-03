// Copyright FaceSoftware. All Rights Reserved.


#include "BTDecorators/BTDecorator_RandomChance.h"

#include "Subsystems/LimenGlobalRandomStreamSubsystem.h"


UBTDecorator_RandomChance::UBTDecorator_RandomChance(): MinRandomValueRange(0), MaxRandomValueRange(0)
{
	NodeName = "Random Chance";
	
}

bool UBTDecorator_RandomChance::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	if (!Super::CalculateRawConditionValue(OwnerComp, NodeMemory))
	{
		return false;
	}

	const float ChanceToSucceed = ChanceToSucceedBlackboardKey.GetValue(OwnerComp.GetBlackboardComponent());
	const float RandomFraction = ULimenGlobalRandomStreamSubsystem::Get()->RandomFloatRange(MinRandomValueRange, MaxRandomValueRange);

	return RandomFraction <= ChanceToSucceed;
}
