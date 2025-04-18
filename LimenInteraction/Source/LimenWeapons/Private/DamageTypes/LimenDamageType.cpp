// Copyright © 2024 FaceSoftware. All rights reserved.


#include "DamageTypes/LimenDamageType.h"


ULimenDamageType::ULimenDamageType()
{
}

float ULimenDamageType::ProcessRawDamage(const float DeltaSeconds, const FDamageParameters& DamageParams)
{
	return DamageParams.DamageValue;
}

bool ULimenDamageType::ShouldStopApplyingDamage() const
{
	return true;
}
