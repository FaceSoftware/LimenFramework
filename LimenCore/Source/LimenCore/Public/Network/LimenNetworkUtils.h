// Fill out your copyright notice in the Description page of Project Settings.

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
