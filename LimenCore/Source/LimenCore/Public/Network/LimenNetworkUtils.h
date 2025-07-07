// Copyright © 2024 FaceSoftware. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Net/Serialization/FastArraySerializer.h"


#define DECLARE_STRUCT_OPS_TYPE_TRAITS(ReplicatedArrayType) \
	template<> \
	struct TStructOpsTypeTraits<ReplicatedArrayType> : public TStructOpsTypeTraitsBase2<ReplicatedArrayType> \
	{ \
		enum  \
		{ \
			WithNetDeltaSerializer = true, \
	   }; \
	};
