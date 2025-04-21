// Copyright Face Software. All Rights Reserved.


#include "Actors/Weapons/LimenLineTraceWeapon.h"

#include "FireMethods/LimenWeaponFireMethod.h"


ALimenLineTraceWeapon::ALimenLineTraceWeapon() : Super()
{
	FireMethodClass = ULimenLineTraceFireMethod::StaticClass();
}
