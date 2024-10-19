// Copyright Face Software. All Rights Reserved.


#include "Attributes/LimenBatteryAttribute.h"

ULimenBatteryAttribute::ULimenBatteryAttribute()
{
	RechargeRate = -1.f; // No auto recharge
	MaxValue = 100.f;
	InitialValue = MaxValue;
}
